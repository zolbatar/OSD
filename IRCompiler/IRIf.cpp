#include "IRCompiler.h"

void IRCompiler::CompileTokenIf(Token* token)
{
	// Conditional statement
	for (auto& t : token->expressions[0])
		CompileToken(t);

	// Check result is int
    auto type = PopType(token);
	if (type!=ValueType::Integer)
		Error(token, "Conditional should return integer");

	// A comparison result on stack, so jump based upon result
	auto b_false = jump_index++;
	auto b_end = jump_index++;
	AddIRWithIndex(IROpcodes::JumpCreateForward, b_false);
	AddIRWithIndex(IROpcodes::JumpCreateForward, b_end);
	AddIR(IROpcodes::StackPopIntOperand1);
	if (token->branches.size()==2) {
		AddIRWithIndex(IROpcodes::JumpOnConditionFalseForward, b_false);
	} else {
		AddIRWithIndex(IROpcodes::JumpOnConditionFalseForward, b_end);
	}

	// True bit
	for (auto& t : token->branches[0])
		CompileToken(t);
	AddIRWithIndex(IROpcodes::JumpForward, b_end);

	// Any else?
	if (token->branches.size()==2) {
		AddIRWithIndex(IROpcodes::JumpDestination, b_false);
		for (auto& t : token->branches[1])
			CompileToken(t);
	}

	AddIRWithIndex(IROpcodes::JumpDestination, b_end);
}
