#include "Parser.h"

void Parser::Parser_REPEAT(Token* t, std::list<Token*>* tokens_out)
{
	// Create statement(s)
	ParseSequenceOfStatements(t, { TokenType::UNTIL });

	// Next up should be an UNTIL
	auto tt = GetToken();
	if (tt->type!=TokenType::UNTIL)
		Error("UNTIL expected", tt);

	// Condition
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[0]);

	tokens_out->push_back(t);
}
