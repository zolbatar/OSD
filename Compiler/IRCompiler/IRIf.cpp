#include "IRCompiler.h"

void IRCompiler::CompileTokenIf(Token *token)
{
    // Conditional statement
    for (auto &t : token->expressions[0])
        CompileToken(t);

    // Check result is int
    auto type = PopType(token);
    if (type != ValueType::Integer)
        Error(token, "Conditional should return integer");

    // A comparison result on stack, so jump based upon result
    auto b_false = jump_index++;
    auto b_end = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_false);
    AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_end);
    AddIR(token, IROpcodes::StackPopIntOperand1);
    if (token->branches.size() == 2)
    {
        AddIRWithIndex(token, IROpcodes::JumpOnConditionFalseForward, b_false);
    }
    else
    {
        AddIRWithIndex(token, IROpcodes::JumpOnConditionFalseForward, b_end);
    }

    // True bit
    for (auto &t : token->branches[0])
        CompileToken(t);
    AddIRWithIndex(token, IROpcodes::JumpForward, b_end);

    // Any else?
    if (token->branches.size() == 2)
    {
        AddIRWithIndex(token, IROpcodes::JumpDestination, b_false);
        for (auto &t : token->branches[1])
            CompileToken(t);
    }

    AddIRWithIndex(token, IROpcodes::JumpDestination, b_end);
}
