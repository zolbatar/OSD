#include "IRCompiler.h"
#include <circle/new.h>
#include <circle/string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288 /**< pi */
#endif

void IRCompiler::CompileToken(Token *token)
{
    switch (token->type)
    {
    case TokenType::END:
        AddIR(token, IROpcodes::End);
        break;
    case TokenType::YIELD:
        AddIR(token, IROpcodes::Yield);
        break;
    case TokenType::STATEMENT_START:
        AddIRWithIndex(token, IROpcodes::StackCheck, token->line_number);
        if (!type_stack.empty())
            Error(token, "Expected empty type stack");
        break;

        // Variables
    case TokenType::DATA:
    case TokenType::DATALABEL:
    case TokenType::READ:
    case TokenType::RESTORE:
        CompileTokenData(token);
        break;

        // Variables
    case TokenType::LOCAL:
        CompileTokenLocalVariable(token);
        break;
    case TokenType::LOCAL_NOINIT:
        CompileTokenLocalNoInitVariable(token);
        break;
    case TokenType::GLOBAL:
        CompileTokenGlobalVariable(token);
        break;
    case TokenType::GLOBAL_NOINIT:
        CompileTokenGlobalNoInitVariable(token);
        break;

        // Looping
    case TokenType::FOR:
        CompileTokenFor(token);
        break;
    case TokenType::NEXT:
        CompileTokenNext(token);
        break;

        // Chrono
    case TokenType::TIME:
        AddIR(token, IROpcodes::StackReturnIntArgument);
        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "TIME");
        type_stack.push(ValueType::Integer);
        break;
    case TokenType::TIMES:
        AddIR(token, IROpcodes::StackReturnStringArgument);
        AddIRWithStringLiteral(token, IROpcodes::CallFunc, "TIME$");
        type_stack.push(ValueType::String);
        break;

        // Maths
    case TokenType::UNARYMINUS:
    case TokenType::ADD:
    case TokenType::SUBTRACT:
    case TokenType::MULTIPLY:
    case TokenType::DIVIDE:
    case TokenType::SHIFT_LEFT:
    case TokenType::SHIFT_RIGHT:
    case TokenType::HAT:
    case TokenType::MOD:
    case TokenType::DIV:
    case TokenType::ABS:
    case TokenType::SQR:
    case TokenType::LN:
    case TokenType::LOG:
    case TokenType::EXP:
    case TokenType::SIN:
    case TokenType::ASN:
    case TokenType::COS:
    case TokenType::ACS:
    case TokenType::TAN:
    case TokenType::ATN:
    case TokenType::RAD:
    case TokenType::DEG:
    case TokenType::SGN:
        CompileTokenMaths(token);
        break;
    case TokenType::PI:
        AddIRFloatLiteral(token, M_PI);
        type_stack.push(ValueType::Float);
        break;

    case TokenType::_TRUE:
        AddIRIntegerLiteral(token, 1);
        type_stack.push(ValueType::Integer);
        break;
    case TokenType::_FALSE:
        AddIRIntegerLiteral(token, 0);
        type_stack.push(ValueType::Integer);
        break;
    case TokenType::AND:
    case TokenType::OR:
    case TokenType::EOR:
    case TokenType::NOT:
        CompileTokenBoolean(token);
        break;

    case TokenType::INT:
    case TokenType::FLOAT:
    case TokenType::STRS:
        CompileTokenConversion(token);
        break;

    case TokenType::ASC:
    case TokenType::CHRS:
    case TokenType::INSTR:
    case TokenType::LEFTS:
    case TokenType::MIDS:
    case TokenType::RIGHTS:
    case TokenType::LEN:
    case TokenType::STRINGS:
        CompileTokenString(token);
        break;

    case TokenType::EQUAL:
    case TokenType::NOTEQUAL:
    case TokenType::LESS:
    case TokenType::LESSEQUAL:
    case TokenType::GREATER:
    case TokenType::GREATEREQUAL:
        CompileTokenComparison(token);
        break;

        // Statements
    case TokenType::SPC:
    case TokenType::TAB:
    case TokenType::PRINTTABBED:
    case TokenType::PRINTTABBEDOFF:
    case TokenType::PRINTNL:
    case TokenType::PRINT:
        CompileTokenPrint(token);
        break;

        // Conditional statements
    case TokenType::CASE:
        CompileTokenCase(token);
        break;
    case TokenType::IF:
        CompileTokenIf(token);
        break;
    case TokenType::REPEAT:
        CompileTokenRepeat(token);
        break;
    case TokenType::WHILE:
        CompileTokenWhile(token);
        break;

        // Literals
    case TokenType::LITERAL_INTEGER:
    case TokenType::LITERAL_FLOAT:
    case TokenType::LITERAL_STRING:
        CompileTokenLiteral(token);
        break;

        // Assignment & swap
    case TokenType::ASSIGNMENT:
        CompileTokenAssignment(token);
        break;
    case TokenType::SWAP:
        CompileTokenSwap(token);
        break;

        // Variables and constants
    case TokenType::CONST:
        CompileTokenConst(token);
        break;
    case TokenType::CONSTANT:
        CompileTokenConstant(token);
        break;
    case TokenType::VARIABLE:
        CompileTokenVariable(token);
        break;

        // Sub-expressions
    case TokenType::SUB_EXPRESSION:
        for (auto &ctoken : token->stack)
            CompileToken(ctoken);
        break;

    case TokenType::DEF:
        CompileTokenDef(token);
        break;
    case TokenType::RETURN:
        CompileTokenReturn(token);
        break;
    case TokenType::PROC_CALL:
        CompileTokenProcCall(token, false);
        break;
    case TokenType::PROC_CALL_EXPR:
        CompileTokenProcCall(token, true);
        break;

        // Unknown?
    default: {
        // Generic function?
        // Save return type
        for (auto &type : token->return_types)
        {
            if (type.IsInteger())
            {
                AddIR(token, IROpcodes::StackReturnIntArgument);
                type_stack.push(ValueType::Integer);
            }
            else if (type.IsFloat())
            {
                AddIR(token, IROpcodes::StackReturnFloatArgument);
                type_stack.push(ValueType::Float);
            }
            else if (type.IsString())
            {
                AddIR(token, IROpcodes::StackReturnStringArgument);
                type_stack.push(ValueType::String);
            }
        }

        // Check each expression in turn for type
        int i = 0;
        for (auto type = token->required_types.rbegin(); type != token->required_types.rend(); ++type)
        {
            for (auto &ctoken : token->expressions[i])
                CompileToken(ctoken);

            auto t = PopType(token);
            if (type->IsInteger())
            {
                EnsureStackIsInteger(token, t);
            }
            else if (type->IsFloat())
            {
                EnsureStackIsFloat(token, t);
            }
            else if (type->IsString())
            {
                if (t != ValueType::String)
                    TypeError(token);
            }
            // type_stack.push(t);
            i++;
        }

        // Set arguments
        for (auto &type : token->required_types)
        {
            if (type.IsInteger())
            {
                AddIR(token, IROpcodes::ArgumentInteger);
            }
            else if (type.IsFloat())
            {
                AddIR(token, IROpcodes::ArgumentFloat);
            }
            else if (type.IsString())
            {
                AddIR(token, IROpcodes::ArgumentString);
            }
        }
        AddIRWithTokenTypeAndStringLiteral(token, IROpcodes::CallFunc, token->name, token->type);
        break;
    }
    }
}

void IRCompiler::Compile(std::list<Token *> *tokens)
{
    // Do forward lookups
    for (auto &token : *tokens)
        ForwardLookups(token);

    for (auto &token : *tokens)
    {
        CompileToken(token);
    }

    // Make sure the end of the global bit ends
    AddIR(tokens->back(), IROpcodes::End);

    // Split global (local init)
    ir_global.splice(ir_global.begin(), ir_global_init);

    // Optimise
    if (optimise)
    {
        RunOptimiser(&ir_global);
        RunOptimiser(&ir);
    }
}

void IRCompiler::CheckForForwardLookup(Token *token)
{
    if (token->type == TokenType::DEF)
    {
        proc_lookup.insert(std::make_pair(token->text, proc_index++));
    }
}

void IRCompiler::ForwardLookups(Token *token)
{
    CheckForForwardLookup(token);
    for (auto &expr : token->expressions)
        for (auto &ctoken : expr)
            ForwardLookups(ctoken);
    for (auto &branch : token->branches)
        for (auto &ctoken : branch)
            ForwardLookups(ctoken);
}

void IRCompiler::Error(Token *token, std::string error)
{
    throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number,
                         token->char_number, error);
}

void IRCompiler::TypeError(Token *token)
{
    throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number,
                         token->char_number, "Unexpected type or syntax error");
}

void IRCompiler::Init_AddIR(Token *token, IROpcodes type)
{
    if (is_global)
        ir_global_init.emplace_back(token, type, 0, 0, 0.0, "", TokenType::NONE);
    else
        current_def_init.emplace_back(token, type, 0, 0, 0.0, "", TokenType::NONE);
}

void IRCompiler::AddIR(Token *token, IROpcodes type)
{
    if (is_global)
        ir_global.emplace_back(token, type, 0, 0, 0.0, "", TokenType::NONE);
    else
        current_def.emplace_back(token, type, 0, 0, 0.0, "", TokenType::NONE);
}

void IRCompiler::Init_AddIRWithIndex(Token *token, IROpcodes type, int64_t index)
{
    if (is_global)
        ir_global_init.emplace_back(token, type, index, 0, 0.0, "", TokenType::NONE);
    else
        current_def_init.emplace_back(token, type, index, 0, 0.0, "", TokenType::NONE);
}

void IRCompiler::AddIRWithIndexAndInteger(Token *token, IROpcodes type, int64_t index, int64_t iv)
{
    if (is_global)
        ir_global.emplace_back(token, type, index, iv, 0.0, "", TokenType::NONE);
    else
        current_def.emplace_back(token, type, index, iv, 0.0, "", TokenType::NONE);
}

void IRCompiler::Init_AddIRWithIndexAndInteger(Token *token, IROpcodes type, int64_t index, int64_t iv)
{
    if (is_global)
        ir_global_init.emplace_back(token, type, index, iv, 0.0, "", TokenType::NONE);
    else
        current_def_init.emplace_back(token, type, index, iv, 0.0, "", TokenType::NONE);
}

void IRCompiler::AddIRWithIndex(Token *token, IROpcodes type, int64_t index)
{
    if (is_global)
        ir_global.emplace_back(token, type, index, 0, 0.0, "", TokenType::NONE);
    else
        current_def.emplace_back(token, type, index, 0, 0.0, "", TokenType::NONE);
}

void IRCompiler::AddIRWithIntegerLiteral(Token *token, IROpcodes type, int64_t v)
{
    if (is_global)
        ir_global.emplace_back(token, type, 0, v, 0.0, "", TokenType::NONE);
    else
        current_def.emplace_back(token, type, 0, v, 0.0, "", TokenType::NONE);
}

void IRCompiler::AddIRWithFloatLiteral(Token *token, IROpcodes type, double v)
{
    if (is_global)
        ir_global.emplace_back(token, type, 0, 0, v, "", TokenType::NONE);
    else
        current_def.emplace_back(token, type, 0, 0, v, "", TokenType::NONE);
}

void IRCompiler::AddIRWithStringLiteral(Token *token, IROpcodes type, std::string v)
{
    if (is_global)
        ir_global.emplace_back(token, type, 0, 0, 0.0, v, TokenType::NONE);
    else
        current_def.emplace_back(token, type, 0, 0, 0.0, v, TokenType::NONE);
}

void IRCompiler::AddIRWithTokenTypeAndStringLiteral(Token *token, IROpcodes type, std::string v, TokenType tt)
{
    if (is_global)
        ir_global.emplace_back(token, type, 0, 0, 0.0, v, tt);
    else
        current_def.emplace_back(token, type, 0, 0, 0.0, v, tt);
}

void IRCompiler::AddIRWithStringLiteralWithInteger(Token *token, IROpcodes type, std::string v, int64_t iv)
{
    if (is_global)
        ir_global.emplace_back(token, type, iv, 0, 0.0, v, TokenType::NONE);
    else
        current_def.emplace_back(token, type, iv, 0, 0.0, v, TokenType::NONE);
}

void IRCompiler::Init_AddIRWithStringLiteralWithIntegerAndIndex(Token *token, IROpcodes type, std::string v, int64_t iv,
                                                                int64_t index)
{
    if (is_global)
        ir_global_init.emplace_back(token, type, index, iv, 0.0, v, TokenType::NONE);
    else
        current_def_init.emplace_back(token, type, index, iv, 0.0, v, TokenType::NONE);
}

void IRCompiler::AddIRWithStringLiteralWithIntegerAndIndex(Token *token, IROpcodes type, std::string v, int64_t iv,
                                                           int64_t index)
{
    if (is_global)
        ir_global.emplace_back(token, type, index, iv, 0.0, v, TokenType::NONE);
    else
        current_def.emplace_back(token, type, index, iv, 0.0, v, TokenType::NONE);
}

void IRCompiler::AddIRIntegerLiteral(Token *token, int64_t v)
{
    if (is_global)
    {
        ir_global.emplace_back(token, IROpcodes::LiteralInteger, 0, v, 0.0, "", TokenType::NONE);
        ir_global.emplace_back(token, IROpcodes::StackPushIntOperand1, 0, v, 0.0, "", TokenType::NONE);
    }
    else
    {
        current_def.emplace_back(token, IROpcodes::LiteralInteger, 0, v, 0.0, "", TokenType::NONE);
        current_def.emplace_back(token, IROpcodes::StackPushIntOperand1, 0, v, 0.0, "", TokenType::NONE);
    }
}

void IRCompiler::AddIRFloatLiteral(Token *token, double v)
{
    if (is_global)
    {
        ir_global.emplace_back(token, IROpcodes::LiteralFloat, 0, 0, v, "", TokenType::NONE);
        ir_global.emplace_back(token, IROpcodes::StackPushFloatOperand1, 0, v, 0.0, "", TokenType::NONE);
    }
    else
    {
        current_def.emplace_back(token, IROpcodes::LiteralFloat, 0, 0, v, "", TokenType::NONE);
        current_def.emplace_back(token, IROpcodes::StackPushFloatOperand1, 0, v, 0.0, "", TokenType::NONE);
    }
}

void IRCompiler::AddIRStringLiteral(Token *token, std::string v)
{
    if (is_global)
    {
        ir_global.emplace_back(token, IROpcodes::LiteralString, 0, 0, 0.0, v, TokenType::NONE);
        ir_global.emplace_back(token, IROpcodes::StackPushStringOperand1, 0, 0, 0.0, "", TokenType::NONE);
    }
    else
    {
        current_def.emplace_back(token, IROpcodes::LiteralString, 0, 0, 0.0, v, TokenType::NONE);
        current_def.emplace_back(token, IROpcodes::StackPushStringOperand1, 0, 0, 0.0, "", TokenType::NONE);
    }
}

void IRCompiler::CheckParamType(Token *token, ValueType wanted_type)
{
    if (type_stack.empty())
        Error(token, "Stack empty, expected value.");
    auto type = PopType(token);
    if (type != wanted_type)
    {
        // Conversion?
        if (wanted_type == ValueType::Integer && type == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
            AddIR(token, IROpcodes::StackPushIntOperand1);
        }
        else if (wanted_type == ValueType::Float && type == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        else
        {
            TypeError(token);
        }
    }
    switch (wanted_type)
    {
    case ValueType::Integer:
        AddIR(token, IROpcodes::ArgumentInteger);
        break;
    case ValueType::Float:
        AddIR(token, IROpcodes::ArgumentFloat);
        break;
    case ValueType::String:
        AddIR(token, IROpcodes::ArgumentString);
        break;
    }
}

void IRCompiler::CheckParamTypeOnly(Token *token, ValueType wanted_type)
{
    if (type_stack.empty())
        Error(token, "Stack empty, expected value.");
    auto type = PopType(token);
    if (type != wanted_type)
    {
        // Conversion?
        if (wanted_type == ValueType::Integer && type == ValueType::Float)
        {
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
            AddIR(token, IROpcodes::StackPushIntOperand1);
        }
        else if (wanted_type == ValueType::Float && type == ValueType::Integer)
        {
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::ConvertOperand1IntToFloat1);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
        }
        else
        {
            TypeError(token);
        }
    }
}

void IRCompiler::SetReturnType(Token *token, ValueType type)
{
    switch (type)
    {
    case ValueType::Integer:
        AddIR(token, IROpcodes::StackPushIntOperand1);
        type_stack.push(ValueType::Integer);
        break;
    case ValueType::Float:
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        type_stack.push(ValueType::Float);
        break;
    case ValueType::String:
        AddIR(token, IROpcodes::StackPushStringOperand1);
        type_stack.push(ValueType::String);
        break;
    }
}