#include "IRCompiler.h"

void IRCompiler::CompileTokenLocalVariable(Token *token)
{

    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateString, token->index);
    else if (token->vtype.IsType())
    {
        Init_AddIRWithIndexAndInteger(token, IROpcodes::VariableLocalCreateType, token->index,
                                      token->vtype.GetType()->size);

        // Add type string vars so we can clear later
        for (auto &t : token->vtype.GetType()->fields)
        {
            if (t.second.type == ValueType::String)
            {
                Init_AddIRWithIndexAndInteger(token, IROpcodes::VariableLocalCreateTypeString, token->index,
                                              t.second.position);
            }
        }
    }
    else
        Error(token, "Why?");

    // Expression value?
    if (!token->expressions.empty())
    {
        for (auto &t : token->expressions[0])
            CompileToken(t);
    }

    // Build IR
    if (token->vtype.IsInteger())
    {
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
    }
    else if (token->vtype.IsFloat())
    {
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
    }
    else if (token->vtype.IsString())
    {
        auto type = PopType(token);
        if (type != ValueType::String)
            TypeError(token);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreString, token->index);
    }
}

void IRCompiler::CompileTokenLocalNoInitVariable(Token *token)
{
    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateString, token->index);
    else
        Error(token, "Why?");
}

void IRCompiler::CompileTokenGlobalVariable(Token *token)
{
    // Expression value?
    if (!token->expressions.empty())
    {
        for (auto &t : token->expressions[0])
            CompileToken(t);
    }

    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateString, token->index);
    else if (token->vtype.IsType())
    {
        auto typ = token->vtype.GetType();
        Init_AddIRWithIndexAndInteger(token, IROpcodes::VariableGlobalCreateType, token->index, typ->size);

        // Add type string vars so we can clear later
        for (auto &t : token->vtype.GetType()->fields)
        {
            if (t.second.type == ValueType::String)
            {
                Init_AddIRWithIndexAndInteger(token, IROpcodes::VariableGlobalCreateTypeString, token->index,
                                              t.second.position);
            }
        }

        // Do we have initialisers?
        if (type_stack.size() > typ->field_names.size())
            Error(token, "Too many initialisers");
        int i = type_stack.size() - 1;
        while (!type_stack.empty())
        {
            auto type = PopType(token);
            auto field = typ->fields.find(typ->field_names[i--])->second;
            switch (field.type)
            {
            case ValueType::Integer:
                EnsureStackIsInteger(token, type);
                AddIR(token, IROpcodes::StackPopIntOperand1);
                AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreIntegerWithOffset, token->index,
                                         field.position);
                break;
            case ValueType::Float:
                EnsureStackIsFloat(token, type);
                AddIR(token, IROpcodes::StackPopFloatOperand1);
                AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreFloatWithOffset, token->index, field.position);
                break;
            case ValueType::String:
                if (type != ValueType::String)
                    TypeError(token);
                AddIR(token, IROpcodes::StackPopStringOperand1);
                AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreStringWithOffset, token->index, field.position);
                break;
            }
        }
    }
    else
        Error(token, "Why?");

    // Build IR
    if (token->vtype.IsInteger())
    {
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
    }
    else if (token->vtype.IsFloat())
    {
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
    }
    else if (token->vtype.IsString())
    {
        auto type = PopType(token);
        if (type != ValueType::String)
            TypeError(token);
        AddIRWithIndex(token, IROpcodes::VariableStoreString, token->index);
    }
}

void IRCompiler::CompileTokenGlobalNoInitVariable(Token *token)
{
    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateString, token->index);
    else
        Error(token, "Why?");
}

void IRCompiler::CompileTokenVariable(Token *token)
{
    if (token->vtype.IsInteger())
    {
        AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        type_stack.push(ValueType::Integer);
    }
    else if (token->vtype.IsFloat())
    {
        AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index);
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        type_stack.push(ValueType::Float);
    }
    else if (token->vtype.IsString())
    {
        AddIRWithIndex(token, IROpcodes::VariableLoadString, token->index);
        AddIR(token, IROpcodes::StackPushStringOperand1);
        type_stack.push(ValueType::String);
    }
    else if (token->vtype.IsType())
    {
        if (token->stack.front()->vtype.IsInteger())
        {
            AddIRWithIndexAndInteger(token, IROpcodes::VariableLoadIntegerWithOffset, token->index,
                                     token->stack.front()->index);
            AddIR(token, IROpcodes::StackPushIntOperand1);
            type_stack.push(ValueType::Integer);
        }
        else if (token->stack.front()->vtype.IsFloat())
        {
            AddIRWithIndexAndInteger(token, IROpcodes::VariableLoadFloatWithOffset, token->index,
                                     token->stack.front()->index);
            AddIR(token, IROpcodes::StackPushFloatOperand1);
            type_stack.push(ValueType::Float);
        }
        else if (token->stack.front()->vtype.IsString())
        {
            AddIRWithIndexAndInteger(token, IROpcodes::VariableLoadStringWithOffset, token->index,
                                     token->stack.front()->index);
            AddIR(token, IROpcodes::StackPushStringOperand1);
            type_stack.push(ValueType::String);
        }
    }
}

void IRCompiler::CompileTokenConst(Token *token)
{
    const_lookup.insert(std::make_pair(token->name, token));
}

void IRCompiler::CompileTokenConstant(Token *token)
{
    auto con = const_lookup.find(token->name)->second;
    if (token->vtype.IsInteger())
    {
        AddIRIntegerLiteral(token, con->integer);
        type_stack.push(ValueType::Integer);
    }
    else if (token->vtype.IsFloat())
    {
        AddIRFloatLiteral(token, con->real);
        type_stack.push(ValueType::Float);
    }
    else if (token->vtype.IsString())
    {
        AddIRStringLiteral(token, con->text);
        type_stack.push(ValueType::String);
    }
}
