#include "IRCompiler.h"

void IRCompiler::CompileTokenWhile(Token *token)
{

    // Conditional statement
    auto b_start = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreate, b_start);
    for (auto &t : token->expressions[0])
        CompileToken(t);

    // Check result is int
    auto type = PopType(token);
    if (type != ValueType::Integer)
        Error(token, "Conditional should return integer");

    auto b_false = jump_index++;
    AddIRWithIndex(token, IROpcodes::JumpCreateForward, b_false);
    AddIR(token, IROpcodes::StackPopIntOperand1);
    AddIRWithIndex(token, IROpcodes::JumpOnConditionFalseForward, b_false);

    // Code gets run if true
    for (auto &t : token->branches[0])
        CompileToken(t);
    AddIRWithIndex(token, IROpcodes::Jump, b_start);

    // End, i.e. condition reached
    AddIRWithIndex(token, IROpcodes::JumpDestination, b_false);
}