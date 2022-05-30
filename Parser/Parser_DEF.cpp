#include "Parser.h"

void Parser::Parser_DEFFastScan(Token* t)
{
	// Get next token, which should be the name
	auto tt = GetToken();
	switch (tt->type) {
		case TokenType::IDENTIFIER:
			t->return_types.push_back(TypeNone());
			break;
		case TokenType::IDENTIFIER_INTEGER:
			t->return_types.push_back(TypeInteger());
			break;
		case TokenType::IDENTIFIER_FLOAT:
			t->return_types.push_back(TypeFloat());
			break;
		case TokenType::IDENTIFIER_STRING:
			t->return_types.push_back(TypeString());
			break;
		default:
			Error("Identifier expected", tt);

	}
	t->text = tt->text;

	// Parens next
	tt = GetToken();
	if (tt->type!=TokenType::LPARENS)
		Error("Opening parentheses expected", tt);

	// Closing parens
	tt = GetToken();
	while (tt->type!=TokenType::RPARENS) {
		// Do we have parameters?
		switch (tt->type) {
			case TokenType::IDENTIFIER:
				Error("Type parameter expected", tt);
			case TokenType::IDENTIFIER_INTEGER:
				t->required_types.push_back(TypeInteger());
				tt->vtype = TypeInteger();
				t->stack.push_back(tt);
				break;
			case TokenType::IDENTIFIER_FLOAT:
				t->required_types.push_back(TypeFloat());
				tt->vtype = TypeFloat();
				t->stack.push_back(tt);
				break;
			case TokenType::IDENTIFIER_STRING:
				t->required_types.push_back(TypeString());
				tt->vtype = TypeString();
				t->stack.push_back(tt);
				break;
		}
		tt = GetToken();
		switch (tt->type) {
			case TokenType::IDENTIFIER_INTEGER:
			case TokenType::IDENTIFIER_FLOAT:
			case TokenType::IDENTIFIER_STRING:
			case TokenType::RPARENS:
				break;
			case TokenType::COMMA:
				tt = GetToken();
				break;
			default:
				SyntaxError(tt);
		}
	}
	PushTokenBack();

	all_defs.insert(std::make_pair(t->text, t));
}

void Parser::Parser_DEF(Token* t, std::list<Token*>* tokens_out)
{
	if (inside_function)
		Error("Nested DEFs not supported", t);

	// Get next token, which should be the name
	auto tt = GetToken();
	switch (tt->type) {
		case TokenType::IDENTIFIER:
			t->return_types.push_back(TypeNone());
			break;
		case TokenType::IDENTIFIER_INTEGER:
			t->return_types.push_back(TypeInteger());
			break;
		case TokenType::IDENTIFIER_FLOAT:
			t->return_types.push_back(TypeFloat());
			break;
		case TokenType::IDENTIFIER_STRING:
			t->return_types.push_back(TypeString());
			break;
		default:
			Error("Identifier expected", tt);

	}
	t->text = tt->text;

	// Parens next
	tt = GetToken();
	if (tt->type!=TokenType::LPARENS)
		Error("Opening parentheses expected", tt);

	// Create parameters as local variables, we'll tie this in at native & IR compilation stage
	local_index = 0;
	local_variables.clear();
	for (auto par : t->stack) {
		CreateLocalVariableNoInit(par);
	}

	// Closing parens
	tt = GetToken();
	while (tt->type!=TokenType::RPARENS)
		tt = GetToken();

	// Body of PROC
	inside_function = true;
	current_def = t;
	ParseSequenceOfStatements(t, { TokenType::ENDDEF });

	// Add explicit return
	Token tn = CreateToken(t, TokenType::RETURN);
	tn.index = 1; // Set as FINAL return
	auto type = current_def->return_types.front();
	if (type.IsNone()) {
	}
	else if (type.IsInteger()) {
		Token tnn = CreateToken(t, TokenType::LITERAL_INTEGER);
		tnn.integer = 0;
		new_tokens.push_back(std::move(tnn));
		tn.expressions.emplace_back(std::list<Token*>());
		tn.expressions[0].push_back(&new_tokens.back());
	}
	else if (type.IsFloat()) {
		Token tnn = CreateToken(t, TokenType::LITERAL_FLOAT);
		tnn.real = 0.0;
		new_tokens.push_back(std::move(tnn));
		tn.expressions.emplace_back(std::list<Token*>());
		tn.expressions[0].push_back(&new_tokens.back());
	}
	else if (type.IsString()) {
		Token tnn = CreateToken(t, TokenType::LITERAL_STRING);
		tnn.text = "";
		new_tokens.push_back(std::move(tnn));
		tn.expressions.emplace_back(std::list<Token*>());
		tn.expressions[0].push_back(&new_tokens.back());
	}
	tn.return_types.push_back(type);
	new_tokens.push_back(std::move(tn));
	t->branches[0].push_back(&new_tokens.back());

	inside_function = false;
	current_def = nullptr;

	// ENDDEF?
	tt = GetToken();
	if (tt->type!=TokenType::ENDDEF)
		Error("Expected ENDDEF", tt);

	tokens_out->push_back(t);
}

void Parser::Parser_RETURN(Token* t, std::list<Token*>* tokens_out)
{
	auto type = current_def->return_types.front();
	t->return_types.push_back(type);
	t->index = 0; // Set as interim return

	// Do we have an expression?
	auto tt = GetToken();
	if (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE) {
		PushTokenBack();

		// What we do here depends on whether we are EXPECTING a return type
		if (type.IsNone()) {
			// Fine
		}
		else if (type.IsInteger()) {
			Token tn = CreateToken(t, TokenType::LITERAL_INTEGER);
			tn.integer = 0;
			new_tokens.push_back(std::move(tn));
			t->expressions.emplace_back(std::list<Token*>());
			t->expressions[0].push_back(&new_tokens.back());
		}
		else if (type.IsFloat()) {
			Token tn = CreateToken(t, TokenType::LITERAL_FLOAT);
			tn.real = 0.0;
			new_tokens.push_back(std::move(tn));
			t->expressions.emplace_back(std::list<Token*>());
			t->expressions[0].push_back(&new_tokens.back());
		}
		else if (type.IsString()) {
			Token tn = CreateToken(t, TokenType::LITERAL_STRING);
			tn.text = "";
			new_tokens.push_back(std::move(tn));
			t->expressions.emplace_back(std::list<Token*>());
			t->expressions[0].push_back(&new_tokens.back());
		}
	}
	else {
		PushTokenBack();
		t->expressions.emplace_back(std::list<Token*>());
		ParseExpression(false, false, &t->expressions[0]);
	}

	tokens_out->push_back(t);
}
