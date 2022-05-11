#include "Parser.h"

void Parser::Parser_PROCCall(Token* t, std::list<Token*>* tokens_out)
{
	t->type = TokenType::PROC_CALL;

	auto f = all_defs.find(t->text);
	if (f==all_defs.end()) {
		Error("DEF not found", t);
	}

	// Types to send? Grab an expression for each of these
	auto top = f->second->return_types.front();
	t->return_types.push_back(top);

	// Parse parameters as expressions, this might well be empty
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[t->expressions.size()-1]);

	// Impossible to know whether number of params match at this time, so copy for later
	for (auto& pt: f->second->required_types)
		t->required_types.push_back(pt);

	tokens_out->push_back(t);
}

