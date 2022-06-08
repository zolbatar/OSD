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
				AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_SPC, "SPC");
			}
			else {
				AddIR(IROpcodes::ArgumentInteger);
				AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_TAB, "TAB");
			}
			break;
		}
		case TokenType::PRINTTABBED:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_Tabbed, "Tabbed On");
			break;
		case TokenType::PRINTTABBEDOFF:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_TabbedOff, "Tabbed Off");
			break;
		case TokenType::PRINTNL:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_NL, "NL");
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
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_integer, "PRINT integer");
								break;
							case ValueType::Float:
								AddIR(IROpcodes::ArgumentFloat);
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_real, "PRINT real");
								break;
							case ValueType::String:
								AddIR(IROpcodes::ArgumentString);
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_string, "PRINT string");
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
