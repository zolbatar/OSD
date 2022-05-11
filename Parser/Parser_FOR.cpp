#include "Parser.h"

void Parser::Parser_FOR(Token* t, std::list<Token*>* tokens_out)
{
	// Get variable
	Token* tt = GetToken();
	t->name = tt->text;

	switch (tt->type) {
		case TokenType::IDENTIFIER_INTEGER:
			t->vtype = TypeInteger();
			break;
		case TokenType::IDENTIFIER_FLOAT:
			t->vtype = TypeFloat();
			break;
		case TokenType::IDENTIFIER_STRING:
			TypeError(tt);
		default:
			Error("Identifier with type expected", tt);
	}

	// Find variable
	auto f = local_variables.find(t->name);
	if (f==local_variables.end()) {

		// Create variable: loop
		tt->vtype = t->vtype;
		CreateLocalVariableNoInit(tt);
		t->index = tt->index;
		tokens_out->push_back(tt);

		// Create variable: iterations
		Token tt2 = CreateToken(t, TokenType::LOCAL);
		tt2.text = t->name+" #iterations";
		tt2.vtype = t->vtype;
		CreateLocalVariableNoInit(&tt2);
		new_tokens.push_back(std::move(tt2));
		tokens_out->push_back(&new_tokens.back());

		// Create variable: step
		tt2 = CreateToken(t, TokenType::LOCAL);
		tt2.text = t->name+" #step";
		tt2.vtype = t->vtype;
		CreateLocalVariableNoInit(&tt2);
		new_tokens.push_back(std::move(tt2));
		tokens_out->push_back(&new_tokens.back());
	}
	else {
		t->vtype = f->second->vtype;
		t->index = f->second->index;
	}

	// Next token should be an equal
	auto tt2 = GetToken();
	if (tt2->type!=TokenType::EQUAL)
		SyntaxError(tt);

	// Value expression is FROM
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[0]);

	// Next token should be a TO
	tt2 = GetToken();
	if (tt2->type!=TokenType::TO)
		SyntaxError(tt2);

	// Value expression is TO
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[1]);

	// Next token should be STEP or end of statement
	tt2 = GetToken();
	if (tt2->type==TokenType::STEP) {
		// Value expression is STEP
		t->expressions.push_back(std::list<Token*>());
		ParseExpression(false, false, &t->expressions[2]);
	}
	else if (tt2->type==TokenType::COLON || tt2->type==TokenType::NEWLINE) {
		// Fine, end
	}
	else
		SyntaxError(tt2);
	tokens_out->push_back(t);
	for_loop_variables.push(t);
}

void Parser::Parser_NEXT(Token* t, std::list<Token*>* tokens_out)
{
	auto tt = GetToken();

	if (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE) {
		// This is fine, we don't support named variables here
	} else {
		Error("NEXT doesn't support named variables", tt);
	}

	// Find last variable
	if (for_loop_variables.empty()) {
		Error("No FOR loop found", t);
	}
	auto var = for_loop_variables.top();
	for_loop_variables.pop();
	t->index = var->index;
	t->vtype = var->vtype;
	tokens_out->push_back(t);
}