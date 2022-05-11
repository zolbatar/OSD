#include "IRCompiler.h"

void IRCompiler::CompileTokenCase(Token* token)
{
	// Compile expression, and get type
	for (auto& t : token->expressions[0])
		CompileToken(t);
    auto type = PopType(token);

	// Create temporary variable to store the expression result
	switch (type) {
		case ValueType::Integer:
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateInteger, token->index);
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
			break;
		case ValueType::Float:
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateFloat, token->index);
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
			break;
		case ValueType::String:
			Init_AddIRWithIndex(IROpcodes::VariableLocalCreateString, token->index);
			AddIR(IROpcodes::StackPopStringOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreString, token->index);
			break;
	}

	// Each token is a separate list of WHEN expressions with a single set of statements
	auto b_end = jump_index++;
	AddIRWithIndex(IROpcodes::JumpCreateForward, b_end);
	for (auto& when : token->stack) {
		auto b_when_code = jump_index++;
		auto b_next_when = jump_index++;
		AddIRWithIndex(IROpcodes::JumpCreateForward, b_when_code);
		AddIRWithIndex(IROpcodes::JumpCreateForward, b_next_when);
		for (auto& expr : when->expressions) {
			for (auto& t : expr) {
				CompileToken(t);
			}
            auto type_check = PopType(token);
			if (type != type_check)
				Error(token, "Mismatched types");

			// Compare against the scratch variable
			switch (type) {
				case ValueType::Integer:
					AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index);
					AddIR(IROpcodes::StackPushIntOperand1);
					AddIR(IROpcodes::StackPopIntOperand1);
					AddIR(IROpcodes::StackPopIntOperand2);
					AddIR(IROpcodes::CompareEqualInt);
					break;
				case ValueType::Float:
					AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index);
					AddIR(IROpcodes::StackPushFloatOperand1);
					AddIR(IROpcodes::StackPopFloatOperand1);
					AddIR(IROpcodes::StackPopFloatOperand2);
					AddIR(IROpcodes::CompareEqualFloat);
					break;
				case ValueType::String:
					AddIRWithIndex(IROpcodes::VariableLoadString, token->index);
					AddIR(IROpcodes::StackPushStringOperand1);
					AddIR(IROpcodes::StackPopStringOperand1);
					AddIR(IROpcodes::StackPopStringOperand2);
					AddIR(IROpcodes::CompareEqualString);
					break;
			}

			// Jump!
			AddIRWithIndex(IROpcodes::JumpOnConditionTrueForward, b_when_code);
		}
		AddIRWithIndex(IROpcodes::JumpForward, b_next_when);

		// Actual code to execute on match
		AddIRWithIndex(IROpcodes::JumpDestination, b_when_code);
		for (auto& t : when->branches[0]) {
			CompileToken(t);
		}
		AddIRWithIndex(IROpcodes::JumpForward, b_end);
		AddIRWithIndex(IROpcodes::JumpDestination, b_next_when);
	}

	// Otherwise
 	if (!token->branches.empty()) {
		for (auto& t : token->branches[0]) {
			CompileToken(t);
		}
	}

 	AddIRWithIndex(IROpcodes::JumpDestination, b_end);
}
