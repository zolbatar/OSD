#include "IRCompiler.h"

void IRCompiler::CompileTokenData(Token* token)
{
	switch (token->type) {
		case TokenType::DATA: {
			for (auto& expr: token->expressions) {

				// Process expression
				for (auto& t: expr)
					CompileToken(t);

				// Get type
				auto type = PopType(token);

				switch (type) {
					case ValueType::Integer:
						AddIR(IROpcodes::StackPopIntOperand1);
						AddIR(IROpcodes::DataInteger);
						break;
					case ValueType::Float:
						AddIR(IROpcodes::StackPopFloatOperand1);
						AddIR(IROpcodes::DataFloat);
						break;
					case ValueType::String:
						AddIR(IROpcodes::StackPopStringOperand1);
						AddIR(IROpcodes::DataString);
						break;
				}
			}
			break;
		}
		case TokenType::DATALABEL: {
			AddIRStringLiteral(token->text);
			AddIR(IROpcodes::StackPopStringOperand1);
			AddIR(IROpcodes::DataLabel);
			break;
		}
		case TokenType::READ: {
			for (auto& t: token->stack) {
				if (t->vtype.IsInteger()) {
					AddIR(IROpcodes::ReadInteger);
					AddIR(IROpcodes::StackPushIntOperand1);
					AddIR(IROpcodes::StackPopIntOperand1);
					AddIRWithIndex(IROpcodes::VariableStoreInteger, t->index);
				}
				else if (t->vtype.IsFloat()) {
					AddIR(IROpcodes::ReadFloat);
					AddIR(IROpcodes::StackPushFloatOperand1);
					AddIR(IROpcodes::StackPopFloatOperand1);
					AddIRWithIndex(IROpcodes::VariableStoreFloat, t->index);
				}
				else if (t->vtype.IsString()) {
					AddIR(IROpcodes::ReadString);
					AddIR(IROpcodes::StackPushStringOperand1);
					AddIR(IROpcodes::StackPopStringOperand1);
					AddIRWithIndex(IROpcodes::VariableStoreString, t->index);
				}
				else
					Error(token, "Why?");
			}
			break;
		}
		case TokenType::RESTORE: {
			AddIRStringLiteral(token->text);
			AddIR(IROpcodes::StackPopStringOperand1);
			AddIR(IROpcodes::Restore);
			break;
		}
		default:
			assert(0);
	}
}
