#include "IRCompiler.h"

void IRCompiler::CompileTokenAssignment(Token *token)
{
    // Expression value
    for (auto &t : token->expressions[0])
        CompileToken(t);

    // Type
    auto type = PopType(token);

    // Build IR
    if (token->vtype.IsInteger())
    {
        EnsureStackIsInteger(token, type);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
    }
    else if (token->vtype.IsFloat())
    {
        EnsureStackIsFloat(token, type);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
    }
    else if (token->vtype.IsString())
    {
        if (type != ValueType::String)
            TypeError(token);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreString, token->index);
    }
    else if (token->vtype.IsType())
    {
        if (token->stack.front()->vtype.IsInteger())
        {
            EnsureStackIsInteger(token, type);
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreIntegerWithOffset, token->index,
                                     token->stack.front()->index);
        }
        else if (token->stack.front()->vtype.IsFloat())
        {
            EnsureStackIsFloat(token, type);
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreFloatWithOffset, token->index,
                                     token->stack.front()->index);
        }
        else if (token->stack.front()->vtype.IsString())
        {
            if (type != ValueType::String)
                TypeError(token);
            AddIR(token, IROpcodes::StackPopStringOperand1);
            AddIRWithIndexAndInteger(token, IROpcodes::VariableStoreStringWithOffset, token->index,
                                     token->stack.front()->index);
        }
    }
}

void IRCompiler::CompileTokenSwap(Token *token)
{
    if (token->vtype.IsInteger())
    {
        AddIRWithIndex(token, IROpcodes::VariableSwapSourceInteger, token->index);
        AddIRWithIndex(token, IROpcodes::VariableSwapDestinationInteger, token->index2);
    }
    else if (token->vtype.IsFloat())
    {
        AddIRWithIndex(token, IROpcodes::VariableSwapSourceFloat, token->index);
        AddIRWithIndex(token, IROpcodes::VariableSwapDestinationFloat, token->index2);
    }
    else if (token->vtype.IsString())
    {
        AddIRWithIndex(token, IROpcodes::VariableSwapSourceString, token->index);
        AddIRWithIndex(token, IROpcodes::VariableSwapDestinationString, token->index2);
    }
    else
        Error(token, "Why?");
}
