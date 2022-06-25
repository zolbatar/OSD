#include "IRCompiler.h"

void IRCompiler::CompileTokenPrint(Token* token)
{
	switch (token->type) {
		case TokenType::SPC:
		case TokenType::TAB: {
			for (auto& expr: token->expressions) {
				for (auto& ctoken: expr)
					CompileToken(ctoken);
			}
			auto v = PopType(token);
			EnsureStackIsInteger(token, v);
			if (token->type==TokenType::SPC) {
				AddIR(IROpcodes::ArgumentInteger);
				AddIRWithStringLiteral(IROpcodes::CallFunc, "SPC");
			}
			else {
				AddIR(IROpcodes::ArgumentInteger);
				AddIRWithStringLiteral(IROpcodes::CallFunc, "TAB");
			}
			break;
		}
		case TokenType::PRINTTABBED:
			AddIRWithStringLiteral(IROpcodes::CallFunc, "Tabbed On");
			break;
		case TokenType::PRINTTABBEDOFF:
			AddIRWithStringLiteral(IROpcodes::CallFunc, "Tabbed Off");
			break;
		case TokenType::PRINTNL:
			AddIRWithStringLiteral(IROpcodes::CallFunc, "NL");
			break;
		case TokenType::PRINT: {
			// Loop through each expression in turn
			for (auto& expr: token->expressions) {
				if (!expr.empty()) {
					for (auto& ctoken: expr)
						CompileToken(ctoken);
					while (!type_stack.empty()) {
						auto type = type_stack.top();
						type_stack.pop();
						switch (type) {
							case ValueType::Integer:
								AddIR(IROpcodes::ArgumentInteger);
								AddIRWithStringLiteral(IROpcodes::CallFunc, "PRINT integer");
								break;
							case ValueType::Float:
								AddIR(IROpcodes::ArgumentFloat);
								AddIRWithStringLiteral(IROpcodes::CallFunc, "PRINT real");
								break;
							case ValueType::String:
								AddIR(IROpcodes::ArgumentString);
								AddIRWithStringLiteral(IROpcodes::CallFunc, "PRINT string");
								break;
						}
					}
				}
			}
			break;
		}
		default:
			Error(token, "Why?");
	}
}
