#include "Parser.h"

void Parser::Parser_PRINT(Token* t, std::list<Token*>* tokens_out)
{
	bool done = false;
	bool new_line = true;
	int i = 0;

	// Set tabbed
	auto nc = CreateToken(t, TokenType::PRINTTABBED);
	new_tokens.push_back(std::move(nc));
	t->expressions.push_back(std::list<Token*>());
	t->expressions[i].push_back(&new_tokens.back());
	i++;

	do {
		t->expressions.push_back(std::list<Token*>());
		ParseExpression(false, false, &t->expressions[i]);
		if (!t->expressions[i].empty())
			new_line = true;
		Token* tt = GetToken();

		// Separator (i.e comma or semicolon)
		if (tt->type==TokenType::COMMA) {
			auto nc = CreateToken(t, TokenType::PRINTTABBED);
			new_tokens.push_back(std::move(nc));
			i++;
			t->expressions.push_back(std::list<Token*>());
			t->expressions[i].push_back(&new_tokens.back());
		}
		else if (tt->type==TokenType::SEMICOLON) {
			auto nc = CreateToken(t, TokenType::PRINTTABBEDOFF);
			new_tokens.push_back(std::move(nc));
			t->expressions[i].push_back(&new_tokens.back());
			new_line = false;
		}
		else if (tt->type==TokenType::TICK) {
			auto tn = CreateToken(t, TokenType::PRINTNL);
			new_tokens.push_back(std::move(nc));
			i++;
			t->expressions.push_back(std::list<Token*>());
			t->expressions[i].push_back(&new_tokens.back());
		}
		else if (tt->type==TokenType::SPC || tt->type==TokenType::TAB) {
			tt->expressions.push_back(std::list<Token*>());
			ParseExpression(false, false, &tt->expressions[0]);
			t->expressions.push_back(std::list<Token*>());
			t->expressions[++i].push_back(tt);
		}
		else {
			PushTokenBack();
			done = true;
		}
		i++;
	}
	while (!done);
	tokens_out->push_back(t);

	// Newline?
	if (new_line) {
		auto nc = CreateToken(t, TokenType::PRINTNL);
		new_tokens.push_back(std::move(nc));
		tokens_out->push_back(&new_tokens.back());
	}
}
