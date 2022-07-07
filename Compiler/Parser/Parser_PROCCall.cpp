#include "Parser.h"

void Parser::Parser_PROCCall(Token* t, std::list<Token*>* tokens_out)
{
	t->type = TokenType::PROC_CALL;

	auto f = all_defs.find(t->text);
	if (f==all_defs.end()) {
		Error("DEF not found", t);
	}

	// Parens next
	auto tt = GetToken();
	if (tt->type!=TokenType::LPARENS)
		Error("Opening parentheses expected", tt);

	// Types to send? Grab an expression for each of these
	auto top = f->second->return_types.front();
	t->return_types.push_back(top);

	// Create space for expressions
	for (auto& pt: f->second->required_types) {
		t->required_types.push_back(pt);
		t->expressions.push_back(std::list<Token*>());
	}

	// Grab parameters
	size_t done = 0;
	int i = 0;
	for (auto& pt: f->second->required_types) {

		// Get expression
		ParseExpression(false, true, &t->expressions[i++]);

		// Do we have a comma (and need one)?
		done++;
		if (done!=t->required_types.size()) {
			tt = GetToken();
			if (tt->type!=TokenType::COMMA)
				Error("Comma expected", tt);
		}
	}

	tt = GetToken();
	if (tt->type!=TokenType::RPARENS)
		Error("Closing parentheses expected", tt);

	if (tokens_out!=NULL)
		tokens_out->push_back(t);
}

