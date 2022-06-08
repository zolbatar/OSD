#include "../Tokeniser/Tokeniser.h"
#include "Parser.h"

bool Parser::IsExpressionTerminator(bool assignment, Token* t, bool last_is_operator)
{
	switch (t->type) {
		case TokenType::IDENTIFIER:
		case TokenType::IDENTIFIER_FLOAT:
		case TokenType::IDENTIFIER_INTEGER:
		case TokenType::IDENTIFIER_STRING:
			if (!last_is_operator)
				return true;
			else
				return false;
		case TokenType::COLON:
		case TokenType::COMMA:
		case TokenType::ELSE:
		case TokenType::ENDDEF:
		case TokenType::ENDIF:
		case TokenType::NEWLINE:
		case TokenType::OF:
		case TokenType::PRINT:
		case TokenType::RETURN:
		case TokenType::SEMICOLON:
		case TokenType::STEP:
		case TokenType::SPC:
		case TokenType::TAB:
		case TokenType::THEN:
		case TokenType::TICK:
		case TokenType::TO:
			return true;
		case TokenType::EQUAL:
			if (assignment)
				return true;
			else
				return false;
		default:
			return false;
	}
	return false;
}

bool Parser::TokenHasParameters(TokenType type)
{
	switch (type) {
		case TokenType::PI:
			return false;
		default:
			return true;
	}
}

int Parser::GetOperatorPrecedence(TokenType type)
{
	switch (type) {
//		case TokenType::FN:

		// Maths
		case TokenType::ABS:
		case TokenType::SQR:
		case TokenType::LN:
		case TokenType::LOG:
		case TokenType::EXP:
		case TokenType::SIN:
		case TokenType::ASN:
		case TokenType::COS:
		case TokenType::ACS:
		case TokenType::TAN:
		case TokenType::ATN:
		case TokenType::RAD:
		case TokenType::DEG:
		case TokenType::SGN:

			// String
		case TokenType::ASC:
		case TokenType::CHRS:
		case TokenType::INSTR:
		case TokenType::LEFTS:
		case TokenType::MIDS:
		case TokenType::RIGHTS:
		case TokenType::LEN:
		case TokenType::STRINGS:
		case TokenType::STRS:

		case TokenType::LPARENS:
		case TokenType::RPARENS:
		case TokenType::UNARYMINUS:
		case TokenType::UNARYPLUS:
		case TokenType::NOT:
			return 100-1;
		case TokenType::HAT:
			return 100-2;
		case TokenType::MOD:
		case TokenType::DIV:
		case TokenType::DIVIDE:
		case TokenType::MULTIPLY:
			return 100-4;
		case TokenType::ADD:
		case TokenType::SUBTRACT:
			return 100-5;
		case TokenType::NOTEQUAL:
		case TokenType::EQUAL:
		case TokenType::GREATEREQUAL:
		case TokenType::LESSEQUAL:
		case TokenType::GREATER:
		case TokenType::LESS:
		case TokenType::SHIFT_LEFT:
		case TokenType::SHIFT_RIGHT:
			return 100-6;
		case TokenType::AND:
			return 100-7;
		case TokenType::EOR:
		case TokenType::OR:
			return 100-8;
		default:
			// We have to assume it's a inbuilt thing like ABS or ACS
			return 100-1;
	}
}

bool Parser::IsLeftAssociative(TokenType type)
{
	if (type==TokenType::HAT || type==TokenType::UNARYMINUS || type==TokenType::UNARYPLUS)
		return true;
	return false;
}

void Parser::EnsureNodeIsToken(Token* t, TokenType oper)
{
	if (t->type!=oper) {
		Error("No operator or invalid operator", t);
	}
	GetToken();
}

void Parser::ParseExpression(bool assignment, bool subexpression, std::list<Token*>* out_stack)
{
	std::stack<Token*> operator_stack;
	bool last_is_operator = true;

	/*
	I am using the shunting yard algorithm here rather than creating an AST.
	Mainly because I'm familiar with it, and it created a nice stack of stuff to
	make compilation easier :-)
	*/

	Token* tt = GetToken();
	while (!IsExpressionTerminator(assignment, tt, last_is_operator)) {
		last_is_operator = false;
		switch (tt->type) {
			case TokenType::_TRUE:
			case TokenType::_FALSE:
			case TokenType::LITERAL_FLOAT:
			case TokenType::LITERAL_INTEGER:
			case TokenType::LITERAL_STRING:
				out_stack->push_back(tt);
				break;
			case TokenType::IDENTIFIER:
				Error("Identifier with type expected", tt);
				break;
			case TokenType::IDENTIFIER_INTEGER:
			case TokenType::IDENTIFIER_FLOAT:
			case TokenType::IDENTIFIER_STRING:
			case TokenType::IDENTIFIER_TYPE: {
				switch (tt->type) {
					case TokenType::IDENTIFIER_INTEGER:
						tt->vtype = TypeInteger();
						break;
					case TokenType::IDENTIFIER_FLOAT:
						tt->vtype = TypeFloat();
						break;
					case TokenType::IDENTIFIER_STRING:
						tt->vtype = TypeString();
						break;
					case TokenType::IDENTIFIER_TYPE: {
						tt->name = std::move(tt->text);
						FindType(tt);
						auto tt2 = GetToken();
						FindField(tt, tt2);
						tt->type = TokenType::VARIABLE;
						tt->stack.push_back(tt2);
						out_stack->push_back(tt);
						tt = GetToken();
						continue;
					}
					default:
						TypeError(tt);
				}

				// Could this be a DEF?
				auto tt2 = GetToken();
				if (tt2->type==TokenType::LPARENS) {
					auto f = all_defs.find(tt->text);
					if (f==all_defs.end()) {
						Error("DEF not found", tt);
					}

					// Types to send? Grab an expression for each of these
					PushTokenBack();
					auto types = &f->second->required_types;
					if (!types->empty()) {
						// Parse parameters as expressions, this might well be empty
						tt->expressions.push_back(std::list<Token*>());
						ParseExpression(false, true, &tt->expressions[tt->expressions.size()-1]);
						//Error("DEF parameters not supported yet", tt);
					}

					// Convert to PROC call
					tt->type = TokenType::PROC_CALL_EXPR;
					auto top = f->second->return_types.front();
					if (top.IsNone())
						Error("This DEF does not return a value", tt);
					tt->return_types.push_back(top);
				}
				else {
					PushTokenBack();

					// We need to find the variable (or constant)
					tt->name = std::move(tt->text);
					auto f = local_variables.find(tt->name);
					if (f==local_variables.end()) {
						auto f = global_variables.find(tt->name);
						if (f==global_variables.end()) {

							// Constant?
							auto cf = constants.find(tt->name);
							if (cf==constants.end()) {

								Error("Unknown variable", tt);
							}
							tt->type = TokenType::CONSTANT;
						}
						else {
							tt->type = TokenType::VARIABLE;
							tt->name = f->second->name;
							tt->index = f->second->index;
						}
					}
					else {
						tt->type = TokenType::VARIABLE;
						tt->name = f->second->name;
						tt->index = f->second->index;
					}
				}
				out_stack->push_back(tt);
				break;
			}

			case TokenType::LPARENS: {
				// We should create a sub-expression for this
				auto nc = CreateToken(tt, TokenType::SUB_EXPRESSION);
				do {

					ParseExpression(false, true, &nc.stack);
					tt = GetToken();
				}
				while (tt->type==TokenType::COMMA);
				if (tt->type!=TokenType::RPARENS)
					PushTokenBack();

				// Push
				new_tokens.push_back(std::move(nc));
				Token* ncc = &new_tokens.back();
				operator_stack.push(ncc);
				break;
			}

			case TokenType::RPARENS: {
				if (subexpression) {
					// Pop everything off operator stack to output
					while (!operator_stack.empty()) {
						out_stack->push_back(operator_stack.top());
						operator_stack.pop();
					}
					PushTokenBack();
					return;
				}
				else {
					Error("Mismatched parentheses", tt);
				}
				break;
			}

			default: {

				// A generic function?
				auto f = generic_functions.find(tt->type);
				if (f!=generic_functions.end()) {
					ParserStatementGeneric(tt, out_stack, &f->second, generic_functions_ptr.find(tt->type)->second, true);
					break;
				}

				last_is_operator = true;
				int prec = GetOperatorPrecedence(tt->type);
				while (!operator_stack.empty() &&
						((GetOperatorPrecedence(operator_stack.top()->type)>prec) ||
								(GetOperatorPrecedence(operator_stack.top()->type)==prec && IsLeftAssociative(tt->type)))) {
					out_stack->push_back(operator_stack.top());
					operator_stack.pop();
				}
				operator_stack.push(tt);
				break;
			}
		}

		// Next token
		tt = GetToken();
	}

	// Push unwanted token back
	PushTokenBack();

	// Pop everything off operator stack to output
	while (!operator_stack.empty()) {
		out_stack->push_back(operator_stack.top());
		operator_stack.pop();
	}
}