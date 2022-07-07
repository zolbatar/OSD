#include "IRCompiler.h"

void IRCompiler::CompileTokenDef(Token *token)
{
    // Create local instruction lists, to append later
    current_def.clear();
    current_def_init.clear();

    is_global = false;
    auto index = proc_lookup.find(token->text)->second;
    Init_AddIRWithStringLiteralWithIntegerAndIndex(token, IROpcodes::PROCStart, token->text,
                                                   token->required_types.size(), index);

    // Parameters? I.e. local variables? Vars first
    int i = 0;
    for (auto &type : token->required_types)
    {
        if (type.IsInteger())
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateInteger, i);
        else if (type.IsFloat())
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateFloat, i);
        else if (type.IsString())
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateString, i);
        i++;
    }

    // Save actual values now
    i = token->required_types.size() - 1;
    for (auto type = token->required_types.rbegin(); type != token->required_types.rend(); ++type)
    {
        if (type->IsInteger())
            Init_AddIRWithIndex(token, IROpcodes::ParameterInt, i);
        else if (type->IsFloat())
            Init_AddIRWithIndex(token, IROpcodes::ParameterFloat, i);
        else if (type->IsString())
            Init_AddIRWithIndex(token, IROpcodes::ParameterString, i);
        i--;
    }

    // Do actual code
    for (auto &branch : token->branches)
        for (auto &ctoken : branch)
            CompileToken(ctoken);

    AddIRWithIndex(token, IROpcodes::PROCEnd, index);
    is_global = true;

    // Add local instruction lists
    ir.splice(ir.end(), current_def_init);
    ir.splice(ir.end(), current_def);
}

void IRCompiler::CompileTokenReturn(Token *token)
{
    auto type = token->return_types.front();
    if (type.IsNone())
    {
        AddIRWithIndex(token, IROpcodes::ReturnNone, token->index);
    }
    else if (type.IsInteger())
    {
        for (auto &t : token->expressions[0])
            CompileToken(t);
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::ReturnInteger, token->index);
    }
    else if (type.IsFloat())
    {
        for (auto &t : token->expressions[0])
            CompileToken(t);
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::ReturnFloat, token->index);
    }
    else if (type.IsString())
    {
        for (auto &t : token->expressions[0])
            CompileToken(t);
        auto type = PopType(token);
        if (type != ValueType::String)
            TypeError(token);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(token, IROpcodes::ReturnString, token->index);
    }
}

void IRCompiler::CompileTokenProcCall(Token *token, bool expression)
{
    auto f = proc_lookup.find(token->text);
    if (f == proc_lookup.end())
    {
        throw DARICException(ExceptionType::COMPILER, filenames->at(token->file_number), token->line_number,
                             token->char_number, "Procedure '" + token->text + "' not found");
    }

    // Return type
    if (expression)
    {
        auto top = token->return_types.front();
        if (top.IsInteger())
        {
            AddIR(token, IROpcodes::StackReturnIntArgument);
            type_stack.push(ValueType::Integer);
        }
        else if (top.IsFloat())
        {
            AddIR(token, IROpcodes::StackReturnFloatArgument);
            type_stack.push(ValueType::Float);
        }
        else if (top.IsString())
        {
            AddIR(token, IROpcodes::StackReturnStringArgument);
            type_stack.push(ValueType::String);
        }
    }

    // Parameters?
    if (token->required_types.size() > 0)
    {

        // Check each expression in turn for type
        int i = 0;
        for (auto type = token->required_types.begin(); type != token->required_types.end(); ++type)
        {
            for (auto &ctoken : token->expressions[i])
                CompileToken(ctoken);

            if (type->IsInteger())
            {
                CheckParamTypeOnly(token, ValueType::Integer);
            }
            else if (type->IsFloat())
            {
                CheckParamTypeOnly(token, ValueType::Float);
            }
            else if (type->IsString())
            {
                CheckParamTypeOnly(token, ValueType::String);
            }
            i++;
        }

        for (auto type = token->required_types.begin(); type != token->required_types.end(); ++type)
        {
            if (type->IsInteger())
            {
                AddIR(token, IROpcodes::ArgumentInteger);
            }
            else if (type->IsFloat())
            {
                AddIR(token, IROpcodes::ArgumentFloat);
            }
            else if (type->IsString())
            {
                AddIR(token, IROpcodes::ArgumentString);
            }
        }
    }

    AddIRWithStringLiteralWithInteger(token, IROpcodes::PROCCall, token->text, f->second);
}
