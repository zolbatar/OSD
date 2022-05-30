#pragma once
#include "Tokens.h"
#include "../OS/OS.h"
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <vector>
#include "Types.h"

enum class TokeniserState {
	NONE,
	NUMBER,
	TEXT,
	STRING,
	SYMBOL,
	REM
};

struct Token {
	// Type
	TokenType type = TokenType::NONE;
	ComplexType vtype = TypeNone();
	int64_t index;
	int64_t index2;

	// Name (for variables etc)
	std::string name;

	// Values
	std::string text;
	int64_t integer = 0;
	double real = 0.0;

	// Generic function
	void* func;

	// Position
	int line_number;
	int file_number;
	int char_number;

	// Children
	std::list<ComplexType> return_types;
	std::list<ComplexType> required_types;
	std::list<Token*> stack;
	std::vector<std::list<Token*>> expressions;
	std::vector<std::list<Token*>> branches;
	void* IR;
};

class Tokeniser {
public:
	Tokeniser();
	void Parse(std::string filename, std::string in);

	std::list<Token>* Tokens()
	{
		return &tokens;
	}

	void PrintTokens(std::list<Token>* tokens, int depth, std::list<std::string>* output);
	void PrintTokensPtr(std::list<Token*>* tokens, int depth, std::list<std::string>* output);

private:
	void PrintToken(Token* token, int depth, std::list<std::string>* output);
	void AddKeyword(TokenDef def);
	void EndOfToken();
	void CreateTokenAndAdd(TokenType type);
	void Error(std::string error);
	void NewLine();
	bool KeywordCheck(bool complete);

	Token CreateToken()
	{
		Token t;
		t.line_number = line_number;
		t.char_number = token_char_number;
		t.file_number = file_number;
		return t;
	}

	// Handlers
	void HandleNone(const char& c);
	void HandleCharacter(const char& c);
	void HandleSymbol(const char& c);
	void HandleNumber(const char& c);
	void HandleText(const char& c);
	void HandleString(const char& c);

	std::string filename;
	int file_number = 0;
	int char_number = 1;
	int token_char_number;
	int line_number = 1;
	std::string search;
	TokeniserState state = TokeniserState::NONE;
	std::list<Token> tokens;
	std::list<TokenDef> current_match_list;

	std::map<char, std::list<TokenDef>> keywords;
	std::map<TokenType, TokenDef> keyword_lookup;
	std::map<std::string, TokenType> symbols;
};