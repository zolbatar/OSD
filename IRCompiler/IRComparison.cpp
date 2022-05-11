#include "IRCompiler.h"

void IRCompiler::CompileTokenComparison(Token* token)
{
	auto type = TypePromotion(token, false);
	switch (token->type) {
		case TokenType::EQUAL:
			GenericComparison(type, IROpcodes::CompareEqualInt, IROpcodes::CompareEqualFloat, IROpcodes::CompareEqualString);
			break;
		case TokenType::NOTEQUAL:
			GenericComparison(type, IROpcodes::CompareNotEqualInt, IROpcodes::CompareNotEqualFloat, IROpcodes::CompareNotEqualString);
			break;
		case TokenType::LESS:
			GenericComparison(type, IROpcodes::CompareLessInt, IROpcodes::CompareLessFloat, IROpcodes::CompareLessString);
			break;
		case TokenType::LESSEQUAL:
			GenericComparison(type, IROpcodes::CompareLessEqualInt, IROpcodes::CompareLessEqualFloat, IROpcodes::CompareLessEqualString);
			break;
		case TokenType::GREATER:
			GenericComparison(type, IROpcodes::CompareGreaterInt, IROpcodes::CompareGreaterFloat, IROpcodes::CompareGreaterString);
			break;
		case TokenType::GREATEREQUAL:
			GenericComparison(type, IROpcodes::CompareGreaterEqualInt, IROpcodes::CompareGreaterEqualFloat, IROpcodes::CompareGreaterEqualString);
			break;
		default:
			Error(token, "Why?");
	}
}

void IRCompiler::GenericComparison(ValueType type, IROpcodes i, IROpcodes r, IROpcodes s)
{
	switch (type) {
		case ValueType::Integer:
			AddIR(IROpcodes::StackPopIntOperand1);
			AddIR(IROpcodes::StackPopIntOperand2);
			AddIR(i);
			AddIR(IROpcodes::StackPushIntOperand1);
			break;
		case ValueType::Float:
			AddIR(IROpcodes::StackPopFloatOperand1);
			AddIR(IROpcodes::StackPopFloatOperand2);
			AddIR(r);
			AddIR(IROpcodes::StackPushIntOperand1);
			break;
		case ValueType::String:
			AddIR(IROpcodes::StackPopStringOperand1);
			AddIR(IROpcodes::StackPopStringOperand2);
			AddIR(s);
			AddIR(IROpcodes::StackPushIntOperand1);
			break;
	}
	type_stack.push(ValueType::Integer);
}

