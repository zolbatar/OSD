#include "IRCompiler.h"

void IRCompiler::CompileTokenFor(Token* token)
{
	if (token->vtype.IsInteger()) {
		// From
		for (auto& t: token->expressions[0])
			CompileToken(t);
		auto type = PopType(token);
		EnsureStackIsInteger(token, type);
		type = ValueType::Integer;
		AddIR(IROpcodes::StackPopIntOperand1);
		AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
		AddIR(IROpcodes::StackPushIntOperand1);

		// To
		for (auto& t: token->expressions[1])
			CompileToken(t);
		type = PopType(token);
		EnsureStackIsInteger(token, type);
		type = ValueType::Integer;

		// Subtract to work out iterations
		AddIR(IROpcodes::StackPopIntOperand2);
		AddIR(IROpcodes::StackPopIntOperand1);
		AddIR(IROpcodes::MathsSubtractInt);
		AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 1 : -1));

		// Step
		if (token->expressions.size()==3) {
			for (auto& t: token->expressions[2])
				CompileToken(t);
			auto type = PopType(token);
			EnsureStackIsInteger(token, type);
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 2 : -2));

			// Adjust iterations
			AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index+(token->index>0 ? 2 : -2));
			AddIR(IROpcodes::StackPushIntOperand1);
			AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index+(token->index>0 ? 1 : -1));
			AddIR(IROpcodes::StackPushIntOperand1);
			AddIR(IROpcodes::StackPopIntOperand2);
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIR(IROpcodes::MathsDiv);
			AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 1 : -1));
		}
		else {
			AddIRIntegerLiteral(1);
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 2 : -2));
		}
	}
	else if (token->vtype.IsFloat()) {
		// From
		for (auto& t: token->expressions[0])
			CompileToken(t);
		auto type = PopType(token);
		EnsureStackIsFloat(token, type);
		type = ValueType::Integer;
		AddIR(IROpcodes::StackPopFloatOperand1);
		AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
		AddIR(IROpcodes::StackPushFloatOperand1);

		// To
		for (auto& t: token->expressions[1])
			CompileToken(t);
		type = PopType(token);
		EnsureStackIsFloat(token, type);
		type = ValueType::Integer;

		// Subtract to work out iterations
		AddIR(IROpcodes::StackPopFloatOperand2);
		AddIR(IROpcodes::StackPopFloatOperand1);
		AddIR(IROpcodes::MathsSubtractFloat);
		AddIR(IROpcodes::ConvertOperand1FloatToInt1);
		AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 1 : -1));

		// Step
		if (token->expressions.size()==3) {
			for (auto& t: token->expressions[2])
				CompileToken(t);
			type = PopType(token);
			EnsureStackIsFloat(token, type);
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index+(token->index>0 ? 2 : -2));

			// Adjust iterations
			AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index+(token->index>0 ? 2 : -2));
			AddIR(IROpcodes::StackPushFloatOperand1);
			AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index+(token->index>0 ? 1 : -1));
			AddIR(IROpcodes::StackPushFloatOperand1);
			AddIR(IROpcodes::StackPopFloatOperand2);
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIR(IROpcodes::MathsDivide);
			AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index+(token->index>0 ? 1 : -1));
		}
		else {
			AddIRFloatLiteral(1.0);
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index+(token->index>0 ? 2 : -2));
		}
	}
	else {
		Error(token, "Unexpected error");
	}

	// Start of loop
	auto b_start = jump_index++;
	AddIRWithIndex(IROpcodes::JumpCreate, b_start);
	for_loop_jumps.push(b_start);
	for_loop_tokens.push(token);
}

void IRCompiler::CompileTokenNext(Token* token)
{
	auto t = for_loop_tokens.top();
	for_loop_tokens.pop();

	if (token->vtype.IsInteger()) {
		// Increase loop var
		AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index+(token->index>0 ? 2 : -2));
		AddIR(IROpcodes::StackPushIntOperand1);
		AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index);
		AddIR(IROpcodes::StackPushIntOperand1);
		AddIR(IROpcodes::StackPopIntOperand2);
		AddIR(IROpcodes::StackPopIntOperand1);
		AddIR(IROpcodes::MathsAddInt);
		AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index);
	}
	else if (token->vtype.IsFloat()) {
		// Increase loop var
		AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index+(token->index>0 ? 2 : -2));
		AddIR(IROpcodes::StackPushFloatOperand1);
		AddIRWithIndex(IROpcodes::VariableLoadFloat, token->index);
		AddIR(IROpcodes::StackPushFloatOperand1);
		AddIR(IROpcodes::StackPopFloatOperand2);
		AddIR(IROpcodes::StackPopFloatOperand1);
		AddIR(IROpcodes::MathsAddFloat);
		AddIRWithIndex(IROpcodes::VariableStoreFloat, token->index);
	}

	// Decrease iterations
	AddIRIntegerLiteral(1);
	AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index+(token->index>0 ? 1 : -1));
	AddIR(IROpcodes::StackPushIntOperand1);
	AddIR(IROpcodes::StackPopIntOperand2);
	AddIR(IROpcodes::StackPopIntOperand1);
	AddIR(IROpcodes::MathsSubtractInt);
	AddIRWithIndex(IROpcodes::VariableStoreInteger, token->index+(token->index>0 ? 1 : -1));

	// Done?
	AddIRIntegerLiteral(0);
	AddIRWithIndex(IROpcodes::VariableLoadInteger, token->index+(token->index>0 ? 1 : -1));
	AddIR(IROpcodes::StackPushIntOperand1);
	AddIR(IROpcodes::StackPopIntOperand2);
	AddIR(IROpcodes::StackPopIntOperand1);
	AddIR(IROpcodes::CompareGreaterEqualInt);
	auto j = for_loop_jumps.top();
	for_loop_jumps.pop();
	AddIRWithIndex(IROpcodes::JumpOnConditionTrue, j);
}
