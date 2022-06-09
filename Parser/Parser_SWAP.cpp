#include "Parser.h"

void Parser::Parser_SWAP(Token* t, std::list<Token*>* tokens_out)
{
	// First variable
	auto tt = GetToken();
	switch (tt->type) {
		case TokenType::IDENTIFIER_INTEGER:
			t->vtype = TypeInteger();
			break;
		case TokenType::IDENTIFIER_FLOAT:
			t->vtype = TypeFloat();
			break;
		case TokenType::IDENTIFIER_STRING:
			t->vtype = TypeString();
			break;
		default:
			Error("Identifier with type expected", tt);
	}
	auto f = local_variables.find(tt->text);
	if (f==local_variables.end())
		Error("Unknown variable '"+tt->name+"'", tt);
	else
		t->index = f->second->index;

	// Next should be a comma
	auto c = GetToken();
	if (c->type!=TokenType::COMMA)
		SyntaxError(tt);

	// Second variable
	auto tt2 = GetToken();
	switch (tt2->type) {
		case TokenType::IDENTIFIER_INTEGER:
		case TokenType::IDENTIFIER_FLOAT:
		case TokenType::IDENTIFIER_STRING:
			break;
		default:
			Error("Identifier with type expected", tt2);
	}
	if (tt->type!=tt2->type)
		Error("Mismatched types", tt2);
	f = local_variables.find(tt2->text);
	if (f==local_variables.end())
		Error("Unknown variable '"+tt2->name+"'", tt2);
	else
		t->index2 = f->second->index;

	tokens_out->push_back(t);
}