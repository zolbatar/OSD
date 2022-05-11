#include "IRCompiler.h"

void IRCompiler::CompileTokenLocalVariable(Token *token) {

    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateString, token->index);
    else if (token->vtype.IsType())
        Init_AddIRWithIndexAndInteger(IROpcodes::VariableLocalCreateType, token->index, token->vtype.GetType()->size);
    else
        Error(token, "Why?");

    // Expression value?
    if (!token->expressions.empty()) {
        for (auto &t: token->expressions[0])
            CompileToken(t);
    }

    // Build IR
    if (token->vtype.IsInteger()) {
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        AddIR(IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
    } else if (token->vtype.IsFloat()) {
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        AddIR(IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
    } else if (token->vtype.IsString()) {
        auto type = PopType(token);
        if (type != ValueType::String)
            TypeError(token);
        AddIR(IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(IROpcodes::VariableStoreString, token->index);
    }
}

void IRCompiler::CompileTokenLocalNoInitVariable(Token *token) {
    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(IROpcodes::VariableLocalCreateString, token->index);
    else
        Error(token, "Why?");
}

void IRCompiler::CompileTokenGlobalVariable(Token *token) {
    // Expression value?
    if (!token->expressions.empty()) {
        for (auto &t: token->expressions[0])
            CompileToken(t);
    }

    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateString, token->index);
    else if (token->vtype.IsType()) {
        auto typ = token->vtype.GetType();
        Init_AddIRWithIndexAndInteger(IROpcodes::VariableGlobalCreateType, token->index, typ->size);

        // Do we have initialisers?
        if (type_stack.size() > typ->field_names.size())
            Error(token, "Too many initialisers");
        int i = type_stack.size() - 1;
        while (!type_stack.empty()) {
            auto type = PopType(token);
            auto field = typ->fields.find(typ->field_names[i--])->second;
            switch (field.type) {
                case ValueType::Integer:
                    EnsureStackIsInteger(token, type);
                    AddIR(IROpcodes::StackPopIntOperand1);
                    AddIRWithIndexAndInteger(IROpcodes::VariableStoreIntegerWithOffset, token->index,
                                             field.position);
                    break;
                case ValueType::Float:
                    EnsureStackIsFloat(token, type);
                    AddIR(IROpcodes::StackPopFloatOperand1);
                    AddIRWithIndexAndInteger(IROpcodes::VariableStoreFloatWithOffset, token->index,
                                             field.position);
                    break;
                case ValueType::String:
                    if (type != ValueType::String)
                        TypeError(token);
                    AddIR(IROpcodes::StackPopStringOperand1);
                    AddIRWithIndexAndInteger(IROpcodes::VariableStoreStringWithOffset, token->index,
                                             field.position);
                    break;
            }
        }

    } else
        Error(token, "Why?");

    // Build IR
    if (token->vtype.IsInteger()) {
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
    } else if (token->vtype.IsFloat()) {
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
    } else if (token->vtype.IsString()) {
        auto type = PopType(token);
        if (type != ValueType::String)
            TypeError(token);
        AddIRWithIndex(IROpcodes::VariableStoreString, token->index);
    }
}

void IRCompiler::CompileTokenGlobalNoInitVariable(Token *token) {
    // Create variable
    if (token->vtype.IsInteger())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateInteger, token->index);
    else if (token->vtype.IsFloat())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateFloat, token->index);
    else if (token->vtype.IsString())
        Init_AddIRWithIndex(IROpcodes::VariableGlobalCreateString, token->index);
    else
        Error(token, "Why?");
}

void IRCompiler::CompileTokenVariable(Token *token) {
    if (token->vtype.IsInteger()) {
        AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index);
        AddIR(IROpcodes::StackPushIntOperand1);
        type_stack.push(ValueType::Integer);
    } else if (token->vtype.IsFloat()) {
        AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index);
        AddIR(IROpcodes::StackPushFloatOperand1);
        type_stack.push(ValueType::Float);
    } else if (token->vtype.IsString()) {
        AddIRWithIndex(IROpcodes::VariableLoadString, token->index);
        AddIR(IROpcodes::StackPushStringOperand1);
        type_stack.push(ValueType::String);
    } else if (token->vtype.IsType()) {
        if (token->stack.front()->vtype.IsInteger()) {
            AddIRWithIndexAndInteger(IROpcodes::VariableLoadIntegerWithOffset, token->index,
                                     token->stack.front()->index);
            AddIR(IROpcodes::StackPushIntOperand1);
            type_stack.push(ValueType::Integer);
        } else if (token->stack.front()->vtype.IsFloat()) {
            AddIRWithIndexAndInteger(IROpcodes::VariableLoadFloatWithOffset, token->index, token->stack.front()->index);
            AddIR(IROpcodes::StackPushFloatOperand1);
            type_stack.push(ValueType::Float);
        } else if (token->stack.front()->vtype.IsString()) {
            AddIRWithIndexAndInteger(IROpcodes::VariableLoadStringWithOffset, token->index,
                                     token->stack.front()->index);
            AddIR(IROpcodes::StackPushStringOperand1);
            type_stack.push(ValueType::String);
        }
    }
}

void IRCompiler::CompileTokenConst(Token *token) {
    const_lookup.insert(std::make_pair(token->name, token));
}

void IRCompiler::CompileTokenConstant(Token *token) {
    auto con = const_lookup.find(token->name)->second;
    if (token->vtype.IsInteger()) {
        AddIRIntegerLiteral(con->integer);
        type_stack.push(ValueType::Integer);
    } else if (token->vtype.IsFloat()) {
        AddIRFloatLiteral(con->real);
        type_stack.push(ValueType::Float);
    } else if (token->vtype.IsString()) {
        AddIRStringLiteral(con->text);
        type_stack.push(ValueType::String);
    }
}
