#include "IRCompiler.h"

void IRCompiler::CompileTokenFor(Token *token)
{
    if (token->vtype.IsInteger())
    {
        // From
        for (auto &t : token->expressions[0])
            CompileToken(t);
        auto type = PopType(token);
        EnsureStackIsInteger(token, type);
        type = ValueType::Integer;
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
        AddIR(token, IROpcodes::StackPushIntOperand1);

        // To
        for (auto &t : token->expressions[1])
            CompileToken(t);
        type = PopType(token);
        EnsureStackIsInteger(token, type);
        type = ValueType::Integer;

        // Subtract to work out iterations
        AddIR(token, IROpcodes::StackPopIntOperand2);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::MathsSubtractInt);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 1 : -1));

        // Step
        if (token->expressions.size() == 3)
        {
            for (auto &t : token->expressions[2])
                CompileToken(t);
            auto type = PopType(token);
            EnsureStackIsInteger(token, type);
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 2 : -2));

            // Adjust iterations
            AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index + (token->index > 0 ? 2 : -2));
            AddIR(token, IROpcodes::StackPushIntOperand1);
            AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index + (token->index > 0 ? 1 : -1));
            AddIR(token, IROpcodes::StackPushIntOperand1);
            AddIR(token, IROpcodes::StackPopIntOperand2);
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIR(token, IROpcodes::MathsDiv);
            AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 1 : -1));
        }
        else
        {
            AddIRIntegerLiteral(token, 1);
            AddIR(token, IROpcodes::StackPopIntOperand1);
            AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 2 : -2));
        }
    }
    else if (token->vtype.IsFloat())
    {
        // From
        for (auto &t : token->expressions[0])
            CompileToken(t);
        auto type = PopType(token);
        EnsureStackIsFloat(token, type);
        type = ValueType::Integer;
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
        AddIR(token, IROpcodes::StackPushFloatOperand1);

        // To
        for (auto &t : token->expressions[1])
            CompileToken(t);
        type = PopType(token);
        EnsureStackIsFloat(token, type);
        type = ValueType::Integer;

        // Subtract to work out iterations
        AddIR(token, IROpcodes::StackPopFloatOperand2);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::MathsSubtractFloat);
        AddIR(token, IROpcodes::ConvertOperand1FloatToInt1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 1 : -1));

        // Step
        if (token->expressions.size() == 3)
        {
            for (auto &t : token->expressions[2])
                CompileToken(t);
            type = PopType(token);
            EnsureStackIsFloat(token, type);
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index + (token->index > 0 ? 2 : -2));

            // Adjust iterations
            AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index + (token->index > 0 ? 2 : -2));
            AddIR(token, IROpcodes::StackPushFloatOperand1);
            AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index + (token->index > 0 ? 1 : -1));
            AddIR(token, IROpcodes::StackPushFloatOperand1);
            AddIR(token, IROpcodes::StackPopFloatOperand2);
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIR(token, IROpcodes::MathsDivide);
            AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index + (token->index > 0 ? 1 : -1));
        }
        else
        {
            AddIRFloatLiteral(token, 1.0);
            AddIR(token, IROpcodes::StackPopFloatOperand1);
            AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index + (token->index > 0 ? 2 : -2));
        }
    }
    else
    {
        Error(token, "Unexpected error");
    }

    // Start of loop
    auto b_start = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreate, b_start);
    for_loop_jumps.push(b_start);
    for_loop_tokens.push(token);
}

void IRCompiler::CompileTokenNext(Token *token)
{
    auto t = for_loop_tokens.top();
    for_loop_tokens.pop();

    if (token->vtype.IsInteger())
    {
        // Increase loop var
        AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index + (token->index > 0 ? 2 : -2));
        AddIR(token, IROpcodes::StackPushIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index);
        AddIR(token, IROpcodes::StackPushIntOperand1);
        AddIR(token, IROpcodes::StackPopIntOperand2);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIR(token, IROpcodes::MathsAddInt);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
    }
    else if (token->vtype.IsFloat())
    {
        // Increase loop var
        AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index + (token->index > 0 ? 2 : -2));
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index);
        AddIR(token, IROpcodes::StackPushFloatOperand1);
        AddIR(token, IROpcodes::StackPopFloatOperand2);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIR(token, IROpcodes::MathsAddFloat);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
    }

    // Decrease iterations
    AddIRIntegerLiteral(token, 1);
    AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index + (token->index > 0 ? 1 : -1));
    AddIR(token, IROpcodes::StackPushIntOperand1);
    AddIR(token, IROpcodes::StackPopIntOperand2);
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIR(token, IROpcodes::MathsSubtractInt);
    AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index + (token->index > 0 ? 1 : -1));

    // Done?
    AddIRIntegerLiteral(token, 0);
    AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index + (token->index > 0 ? 1 : -1));
    AddIR(token, IROpcodes::StackPushIntOperand1);
    AddIR(token, IROpcodes::StackPopIntOperand2);
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIR(token, IROpcodes::CompareGreaterEqualInt);
    auto j = for_loop_jumps.top();
    for_loop_jumps.pop();
    AddIRWithIndex(token, IROpcodes::JumpOnConditionTrue, j);
}
