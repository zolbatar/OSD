#include "IRCompiler.h"

void IRCompiler::CompileTokenCase(Token *token)
{
    // Compile expression, and get type
    for (auto &t : token->expressions[0])
        CompileToken(t);
    auto type = PopType(token);

    // Create temporary variable to store the expression result
    switch (type)
    {
    case ValueType::Integer:
        if (token->index > 0)
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateInteger, token->index);
        else
            Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateInteger, token->index);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
        break;
    case ValueType::Float:
        if (token->index > 0)
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateFloat, token->index);
        else
            Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateFloat, token->index);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
        break;
    case ValueType::String:
        if (token->index > 0)
            Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateString, token->index);
        else
            Init_AddIRWithIndex(token, IROpcodes::VariableGlobalCreateString, token->index);
        AddIR(token, IROpcodes::StackPopStringOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreString, token->index);
        break;
    }

    // Each token is a separate list of WHEN expressions with a single set of statements
    auto b_end = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_end);
    for (auto &when : token->stack)
    {
        auto b_when_code = jump_index++;
        auto b_next_when = jump_index++;
        AddIRWithIndex(when, IROpcodes::JumpCreateForward, b_when_code);
        AddIRWithIndex(when, IROpcodes::JumpCreateForward, b_next_when);
        for (auto &expr : when->expressions)
        {
            for (auto &t : expr)
            {
                CompileToken(t);
            }
            auto type_check = PopType(token);
            if (type != type_check)
                Error(token, "Mismatched types");

            // Compare against the scratch variable
            switch (type)
            {
            case ValueType::Integer:
                AddIRWithIndex(when, IROpcodes::VariableLoadInteger, token->index);
                AddIR(when, IROpcodes::StackPushIntOperand1);
                AddIR(when, IROpcodes::StackPopIntOperand1);
                AddIR(when, IROpcodes::StackPopIntOperand2);
                AddIR(when, IROpcodes::CompareEqualInt);
                break;
            case ValueType::Float:
                AddIRWithIndex(when, IROpcodes::VariableLoadFloat, token->index);
                AddIR(when, IROpcodes::StackPushFloatOperand1);
                AddIR(when, IROpcodes::StackPopFloatOperand1);
                AddIR(when, IROpcodes::StackPopFloatOperand2);
                AddIR(when, IROpcodes::CompareEqualFloat);
                break;
            case ValueType::String:
                AddIRWithIndex(when, IROpcodes::VariableLoadString, token->index);
                AddIR(when, IROpcodes::StackPushStringOperand1);
                AddIR(when, IROpcodes::StackPopStringOperand1);
                AddIR(when, IROpcodes::StackPopStringOperand2);
                AddIR(when, IROpcodes::CompareEqualString);
                break;
            }

            // Jump!
            AddIRWithIndex(when, IROpcodes::JumpOnConditionTrueForward, b_when_code);
        }
        AddIRWithIndex(when, IROpcodes::JumpForward, b_next_when);

        // Actual code to execute on match
        AddIRWithIndex(when, IROpcodes::JumpDestination, b_when_code);
        for (auto &t : when->branches[0])
        {
            CompileToken(t);
        }
        AddIRWithIndex(when, IROpcodes::JumpForward, b_end);
        AddIRWithIndex(when, IROpcodes::JumpDestination, b_next_when);
    }

    // Otherwise
    if (!token->branches.empty())
    {
        for (auto &t : token->branches[0])
        {
            CompileToken(t);
        }
    }

    AddIRWithIndex(token, IROpcodes::JumpDestination, b_end);
}
