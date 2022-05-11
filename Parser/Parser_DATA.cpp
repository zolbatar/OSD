#include "Parser.h"

void Parser::Parser_DATA(Token* t, std::list<Token*>* tokens_out)
{
	while (true) {

		// Get next token, which should be an expression
		t->expressions.push_back(std::list<Token*>());
		ParseExpression(false, false, &t->expressions[t->expressions.size()-1]);

		// Next is either a comma to suggest another, or a colon/newline to end
		auto tt = GetToken();
		if (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE)
			break;
	}
	tokens_out->push_back(t);
}

void Parser::Parser_DATALABEL(Token* t, std::list<Token*>* tokens_out)
{
	// Get next token, which should be a literal string
	auto tt = GetToken();
	if (tt->type!=TokenType::LITERAL_STRING)
		Error("Expected a literal string label", tt);

	t->text = tt->text;
	tokens_out->push_back(t);
}

void Parser::Parser_RESTORE(Token* t, std::list<Token*>* tokens_out)
{
	// Get next token, which should be a literal string
	auto tt = GetToken();
	if (tt->type!=TokenType::LITERAL_STRING)
		Error("Expected a literal string label", tt);

	t->text = tt->text;
	tokens_out->push_back(t);
}

void Parser::Parser_READ(Token* t, std::list<Token*>* tokens_out)
{
	while (true) {
		auto tt = GetToken();
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
            default:
                SyntaxError(tt);
		}
        tt->type = TokenType::VARIABLE;
		tt->name = std::move(tt->text);
		FindOrCreateVariable(tt, tokens_out, false);
		t->stack.push_back(tt);

		// Next token
		tt = GetToken();
		if (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE)
			break;
		else if (tt->type==TokenType::COMMA) {
			// All good
		}
		else
			SyntaxError(tt);
	}
	tokens_out->push_back(t);
}
