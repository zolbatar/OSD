#include "Parser.h"
#include "../Exception/DARICException.h"
#include <tuple>

#ifndef CLION
#include <circle/timer.h>
extern CTimer *timer;
#endif

void Parser::Error(std::string message, Token* token)
{
	throw DARICException(ExceptionType::PARSER, token->line_number, token->char_number, message);
}

void Parser::SyntaxError(Token* token)
{
	throw DARICException(ExceptionType::PARSER, token->line_number, token->char_number, "Syntax error");
}

void Parser::TypeError(Token* token)
{
	throw DARICException(ExceptionType::PARSER, token->line_number, token->char_number, "Unexpected type");
}

Parser::Parser()
{
	// 2D
	generic_functions.insert(std::make_pair(TokenType::SCREENWIDTH, fp{ TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::SCREENWIDTH, (void*)&call_2D_screenwidth));
	generic_functions.insert(std::make_pair(TokenType::SCREENHEIGHT, fp{ TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::SCREENHEIGHT, (void*)&call_2D_screenheight));
	generic_functions.insert(std::make_pair(TokenType::COLOUR, fp{ TypeNone(), TypeInteger(), TypeInteger(), TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::COLOUR, (void*)&call_2D_colour));
	generic_functions.insert(std::make_pair(TokenType::COLOURBG, fp{ TypeNone(), TypeInteger(), TypeInteger(), TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::COLOURBG, (void*)&call_2D_colourbg));
	generic_functions.insert(std::make_pair(TokenType::CLS, fp{ TypeNone() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::CLS, (void*)&call_2D_cls));
	generic_functions.insert(std::make_pair(TokenType::PLOT, fp{ TypeNone(), TypeInteger(), TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::PLOT, (void*)&call_2D_plot));
	generic_functions.insert(std::make_pair(TokenType::LINE, fp{ TypeNone(), TypeInteger(), TypeInteger(), TypeInteger(), TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::LINE, (void*)&call_2D_line));

	// Keyboard
	generic_functions.insert(std::make_pair(TokenType::INKEY, fp{ TypeInteger(), TypeInteger() }));
	generic_functions_ptr.insert(std::make_pair(TokenType::INKEY, (void*)&call_INKEY));
}

bool Parser::Parse(bool optimise, std::list<Token>* tokens)
{
	// Reset
	final_tokens.clear();
	new_tokens.clear();
	constants.clear();
	while (!for_loop_variables.empty())
		for_loop_variables.pop();
	this->optimise = optimise;
	it = tokens->begin();

	// Fast scan of all DEFs for forward lookup
	// Top level
	Token* t;
	do {
		t = GetToken();
		switch (t->type) {
			case TokenType::DEF:
				Parser_DEFFastScan(t);
				break;
		}
	}
	while (t->type!=TokenType::NONE);
	it = tokens->begin();

	// Top level
	while (1) {
		Token* t = GetToken();
		switch (t->type) {
			case TokenType::COMMA:
			case TokenType::NEWLINE:
				// Ignore
				break;
			case TokenType::NONE:
				// We are done
				return true;

				// PROCs
			case TokenType::DEF:
				Parser_DEF(t, &final_tokens);
				break;

				// Constants
			case TokenType::CONST:
				Parser_CONST(t, &final_tokens);
				break;

				// DATA
			case TokenType::DATA:
				Parser_DATA(t, &final_tokens);
				break;
			case TokenType::DATALABEL:
				Parser_DATALABEL(t, &final_tokens);
				break;

			default:
				ParseStatement(t, &final_tokens);
		}
	}
	return false;
}

void Parser::ParseSequenceOfStatements(Token* t, std::set<TokenType> block_terminator)
{
	// Do we have any leading colons or newlines?
	auto tt = GetToken();
	bool single_line = block_terminator.count(TokenType::NEWLINE) > 0;
	if (!single_line) {
		while (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE)
			tt = GetToken();
	}
	else {
		while (tt->type==TokenType::COLON)
			tt = GetToken();
	}
	PushTokenBack();

	// Create new branch
	t->branches.push_back(std::list<Token*>());
	auto to = &t->branches[t->branches.size()-1];
	do {
		tt = GetToken();
		ParseStatement(tt, to);

		// Trailing colons??
		tt = GetToken();
		if (single_line) {
			while (tt->type==TokenType::COLON)
				tt = GetToken();
		}
		else {
			while (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE)
				tt = GetToken();
		}
		PushTokenBack();
	}
	while (single_line ? !IsStatementTerminator(false, tt) : !IsBlockTerminator(false, tt, block_terminator));

	// Trailing colons or new lines??
	while (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE)
		tt = GetToken();

	PushTokenBack();
}

void Parser::ParserStatementGeneric(Token* t, std::list<Token*>* tokens_out, fp* parameters, void* func, bool expression)
{
	// Parentheses are optional
	auto tt = GetToken();
	t->func = func;
	bool optional_parens = true;
	if (tt->type!=TokenType::LPARENS) {
		PushTokenBack();
		optional_parens = false;
	}

	// Create space for all expressions
	bool first = true;
	for (auto p: *parameters) {
		if (first) {
			// Return type
			if (expression) {
				t->return_types.push_back(p);
			}
			first = false;
		}
		else {
			t->required_types.push_back(p);
			t->expressions.push_back(std::list<Token*>());
		}
	}

	// Get each parameter in turn (working backwards)
	size_t done = 0;
	int i = t->required_types.size()-1;
	for (auto p: t->required_types) {

		// Get expression
		ParseExpression(false, optional_parens, &t->expressions[i--]);

		// Do we have a comma (and need one)?
		done++;
		if (done!=t->required_types.size()) {
			tt = GetToken();
			if (tt->type!=TokenType::COMMA)
				Error("Comma expected", tt);
		}
	}

	// Parentheses are optional
	if (optional_parens) {
		tt = GetToken();
		if (tt->type!=TokenType::RPARENS)
			Error("Closing parentheses expected", tt);
	}

	tokens_out->push_back(t);
}

void Parser::ParseStatement(Token* t, std::list<Token*>* tokens_out)
{
	if (!optimise) {
		auto tt = CreateToken(t, TokenType::STATEMENT_START);
		new_tokens.push_back(std::move(tt));
		tokens_out->push_back(&new_tokens.back());
	}

	switch (t->type) {
		case TokenType::CASE:
			Parser_CASE(t, tokens_out);
			break;
		case TokenType::END:
			tokens_out->push_back(t);
			break;
		case TokenType::ENDDEF:
			// Ignore here
			PushTokenBack();
			break;
		case TokenType::FOR:
			Parser_FOR(t, tokens_out);
			break;
		case TokenType::IDENTIFIER:
		case TokenType::IDENTIFIER_FLOAT:
		case TokenType::IDENTIFIER_INTEGER:
		case TokenType::IDENTIFIER_STRING:
		case TokenType::IDENTIFIER_TYPE: {
			// Either a PROC call or an assignment, depending on whether there are parentheses or not
			auto tt = GetToken();
			PushTokenBack();
			if (tt->type==TokenType::LPARENS) {
				Parser_PROCCall(t, tokens_out);
			}
			else {
				ParserAssignment(t, tokens_out, false);
			}
			break;
		}
		case TokenType::IF:
			Parser_IF(t, tokens_out);
			break;
		case TokenType::LET:
			t = GetToken();
			ParserAssignment(t, tokens_out, false);
			break;
		case TokenType::LOCAL:
			if (!inside_function)
				Error("LOCAL only valid within a DEF", t);
			t = GetToken();
			ParserAssignment(t, tokens_out, true);
			break;
		case TokenType::NEXT:
			Parser_NEXT(t, tokens_out);
			break;
		case TokenType::PRINT:
			Parser_PRINT(t, tokens_out);
			break;
		case TokenType::READ:
			Parser_READ(t, tokens_out);
			break;
		case TokenType::REM:
			break;
		case TokenType::REPEAT:
			Parser_REPEAT(t, tokens_out);
			break;
		case TokenType::RESTORE:
			Parser_RESTORE(t, tokens_out);
			break;
		case TokenType::RETURN:
			Parser_RETURN(t, tokens_out);
			break;
		case TokenType::SWAP:
			Parser_SWAP(t, tokens_out);
			break;
		case TokenType::TYPE:
			Parser_TYPE(t, tokens_out);
			break;
		case TokenType::WHILE:
			Parser_WHILE(t, tokens_out);
			break;
		case TokenType::YIELD:
			tokens_out->push_back(t);
			break;

		default: {
			// A generic function?
			auto f = generic_functions.find(t->type);
			if (f!=generic_functions.end()) {
				ParserStatementGeneric(t, tokens_out, &f->second, generic_functions_ptr.find(t->type)->second, false);
				break;
			}

			// Not, so error
			Error("Syntax error", t);
		}
	}
}

Token* Parser::GetToken()
{
	Token* t = &*it;
	previous = it;
	it++;
/*	if (t->line_number == 27 && t->char_number == 10) {
		int a = 1;
	}*/
	return t;
}

void Parser::PushTokenBack()
{
	it = previous;
}

bool Parser::IsStatementTerminator(bool assignment, Token* t)
{
	switch (t->type) {
		case TokenType::NEWLINE:
		case TokenType::COLON:
		case TokenType::THEN:
		case TokenType::ELSE:
			return true;
		default:
			return false;
	}
	return false;
}

bool Parser::IsBlockTerminator(bool assignment, Token* t, std::set<TokenType> block_terminator)
{
	return block_terminator.count(t->type) > 0;
}

void Parser::CreateLocalVariable(Token* tt, bool init)
{
	tt->index = local_index++;
	tt->type = TokenType::LOCAL;
	tt->name = std::move(tt->text);
	tt->text = "";
	local_variables.insert(std::make_pair(tt->name, tt));

	// Set default expression to zero out
	if (init) {
		auto tn = CreateToken(tt, TokenType::NONE);
		if (tt->vtype.IsInteger()) {
			tn.type = TokenType::LITERAL_INTEGER;
			tn.integer = 0;
		}
		else if (tt->vtype.IsFloat()) {
			tn.type = TokenType::LITERAL_FLOAT;
			tn.real = 0;
		}
		else if (tt->vtype.IsString()) {
			tn.type = TokenType::LITERAL_STRING;
			tn.text = "";
		}
		else {
			TypeError(tt);
		}
		new_tokens.push_back(std::move(tn));
		tt->expressions.push_back(std::list<Token*>());
		tt->expressions[0].push_back(&new_tokens.back());
	}
	auto f = local_variables.find(tt->name);
	tt->index = f->second->index;
}

void Parser::CreateGlobalVariable(Token* tt, bool init)
{
	tt->index = global_index--;
	tt->type = TokenType::GLOBAL;
	tt->name = std::move(tt->text);
	tt->text = "";
	global_variables.insert(std::make_pair(tt->name, tt));

	// Set default expression to zero out
	if (init) {
		auto tn = CreateToken(tt, TokenType::NONE);
		if (tt->vtype.IsInteger()) {
			tn.type = TokenType::LITERAL_INTEGER;
			tn.integer = 0;
		}
		else if (tt->vtype.IsFloat()) {
			tn.type = TokenType::LITERAL_FLOAT;
			tn.real = 0;
		}
		else if (tt->vtype.IsString()) {
			tn.type = TokenType::LITERAL_STRING;
			tn.text = "";
		}
		else
			TypeError(tt);
		new_tokens.push_back(std::move(tn));
		tt->expressions.push_back(std::list<Token*>());
		tt->expressions[0].push_back(&new_tokens.back());
	}
	auto f = global_variables.find(tt->name);
	tt->index = f->second->index;
}

void Parser::CreateLocalVariableNoInit(Token* tt)
{
	tt->index = local_index++;
	tt->type = TokenType::LOCAL_NOINIT;
	tt->name = std::move(tt->text);
	tt->text = "";
	local_variables.insert(std::make_pair(tt->name, tt));
	auto f = local_variables.find(tt->name);
	tt->index = f->second->index;
}

void Parser::CreateGlobalVariableNoInit(Token* tt)
{
	tt->index = global_index--;
	tt->type = TokenType::GLOBAL_NOINIT;
	tt->name = std::move(tt->text);
	tt->text = "";
	global_variables.insert(std::make_pair(tt->name, tt));
	auto f = global_variables.find(tt->name);
	tt->index = f->second->index;
}

Token* Parser::CreateNewLocalVariableNoInit(Token tt)
{
	tt.index = local_index++;
	tt.type = TokenType::LOCAL_NOINIT;
	tt.name = std::move(tt.text);
	tt.text = "";
	new_tokens.push_back(std::move(tt));
	Token* t = &new_tokens.back();
	local_variables.insert(std::make_pair(t->name, t));
	auto f = local_variables.find(tt.name);
	tt.index = f->second->index;
	return t;
}

Token* Parser::CreateNewGlobalVariableNoInit(Token tt)
{
	tt.index = global_index--;
	tt.type = TokenType::GLOBAL_NOINIT;
	tt.name = std::move(tt.text);
	tt.text = "";
	new_tokens.push_back(std::move(tt));
	Token* t = &new_tokens.back();
	global_variables.insert(std::make_pair(t->name, t));
	auto f = global_variables.find(t->name);
	t->index = f->second->index;
	return t;
}

Token Parser::CreateToken(Token* parent, TokenType t)
{
	Token nc;
	nc.type = t;
	nc.line_number = parent->line_number;
	nc.char_number = parent->char_number;
	nc.file_number = parent->line_number;
	return nc;
}

ComplexType TypeNone()
{
	ComplexType t;
	t.None();
	return t;
}

ComplexType TypeInteger()
{
	ComplexType t;
	t.Integer();
	return t;
}

ComplexType TypeFloat()
{
	ComplexType t;
	t.Float();
	return t;
}

ComplexType TypeString()
{
	ComplexType t;
	t.String();
	return t;
}

ComplexType TypeStruct(Type* type)
{
	ComplexType t;
	t.Struct(type);
	return t;
}