#include "Parser.h"

void Parser::Parser_IF(Token* t, std::list<Token*>* tokens_out)
{
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[0]);

	// Now at statement stage
	Token* tt = GetToken();

	// Is there an THEN? If so it's a multi-line thing
	if (tt->type==TokenType::THEN) {

		// Next up should be a newline
		tt = GetToken();
		if (tt->type!=TokenType::NEWLINE)
			Error("New line expected", tt);

		// Create statement(s)
		ParseSequenceOfStatements(t, false);

		tt = GetToken();
		if (tt->type==TokenType::ELSE) {
			ParseSequenceOfStatements(t, false);
		}

		// Should finish with an ENDIF
		tt = GetToken();
		if (tt->type!=TokenType::ENDIF)
			Error("ENDIF expected", tt);
	}
	else {
		// It's not a THEN, so it's a proper statement, keep grabbing until we hit end of line
		PushTokenBack();

		// Create statement(s)
		ParseSequenceOfStatements(t, true);
		tt = GetToken();
		if (tt->type==TokenType::ELSE) {
			ParseSequenceOfStatements(t, true);
		}
	}
	tokens_out->push_back(t);
}