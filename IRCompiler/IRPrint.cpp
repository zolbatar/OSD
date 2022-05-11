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
				AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_SPC);
			}
			else {
				AddIR(IROpcodes::ArgumentInteger);
				AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_TAB);
			}
			break;
		}
		case TokenType::PRINTTABBED:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_Tabbed);
			break;
		case TokenType::PRINTTABBEDOFF:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_TabbedOff);
			break;
		case TokenType::PRINTNL:
			AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_NL);
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
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_integer);
								break;
							case ValueType::Float:
								AddIR(IROpcodes::ArgumentFloat);
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_real);
								break;
							case ValueType::String:
								AddIR(IROpcodes::ArgumentString);
								AddIRWithAddress(IROpcodes::CallFunc, (void*)&call_PRINT_string);
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
