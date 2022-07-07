#include "IRCompiler.h"

void IRCompiler::CompileTokenRepeat(Token *token)
{
    auto b_start = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreate, b_start);

    // Code always gets run
    for (auto &t : token->branches[0])
        CompileToken(t);

    // Conditional statement
    for (auto &t : token->expressions[0])
        CompileToken(t);

    // Check result is int
    auto type = PopType(token);
    if (type != ValueType::Integer)
        Error(token, "Conditional should return integer");

    // Do jump
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIRWithIndex(token, IROpcodes::JumpOnConditionFalse, b_start);
}