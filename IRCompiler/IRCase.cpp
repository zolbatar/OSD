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
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateInteger, token->index);
        AddIR(token, IROpcodes::StackPopIntOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreInteger, token->index);
        break;
    case ValueType::Float:
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateFloat, token->index);
        AddIR(token, IROpcodes::StackPopFloatOperand1);
        AddIRWithIndex(token, IROpcodes::VariableStoreFloat, token->index);
        break;
    case ValueType::String:
        Init_AddIRWithIndex(token, IROpcodes::VariableLocalCreateString, token->index);
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
        AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_when_code);
        AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_next_when);
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
                AddIRWithIndex(token, IROpcodes::VariableLoadInteger, token->index);
                AddIR(token, IROpcodes::StackPushIntOperand1);
                AddIR(token, IROpcodes::StackPopIntOperand1);
                AddIR(token, IROpcodes::StackPopIntOperand2);
                AddIR(token, IROpcodes::CompareEqualInt);
                break;
            case ValueType::Float:
                AddIRWithIndex(token, IROpcodes::VariableLoadFloat, token->index);
                AddIR(token, IROpcodes::StackPushFloatOperand1);
                AddIR(token, IROpcodes::StackPopFloatOperand1);
                AddIR(token, IROpcodes::StackPopFloatOperand2);
                AddIR(token, IROpcodes::CompareEqualFloat);
                break;
            case ValueType::String:
                AddIRWithIndex(token, IROpcodes::VariableLoadString, token->index);
                AddIR(token, IROpcodes::StackPushStringOperand1);
                AddIR(token, IROpcodes::StackPopStringOperand1);
                AddIR(token, IROpcodes::StackPopStringOperand2);
                AddIR(token, IROpcodes::CompareEqualString);
                break;
            }

            // Jump!
            AddIRWithIndex(token, IROpcodes::JumpOnConditionTrueForward, b_when_code);
        }
        AddIRWithIndex(token, IROpcodes::JumpForward, b_next_when);

        // Actual code to execute on match
        AddIRWithIndex(token, IROpcodes::JumpDestination, b_when_code);
        for (auto &t : when->branches[0])
        {
            CompileToken(t);
        }
        AddIRWithIndex(token, IROpcodes::JumpForward, b_end);
        AddIRWithIndex(token, IROpcodes::JumpDestination, b_next_when);
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
