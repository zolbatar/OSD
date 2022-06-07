#include "Tokeniser.h"
#include <algorithm>
#include <iostream>

void Tokeniser::HandleText(const char& c)
{
	if (std::isalnum(c) || c=='_') {
		search += c;
		if (KeywordCheck(false))
			return;
	}
	else if (c=='%') {
		search += c;

		// This might be a keyword ending in % so check that first
		if (KeywordCheck(true))
			return;

		Token t = CreateToken();
		t.type = TokenType::IDENTIFIER_INTEGER;
		t.text = search;
		tokens.push_back(std::move(t));
		EndOfToken();
	}
	else if (c=='!') {
		search += c;

		// This might be a keyword ending in % so check that first
		if (KeywordCheck(true))
			return;

		Token t = CreateToken();
		t.type = TokenType::IDENTIFIER_TYPE;
		t.text = search;
		tokens.push_back(std::move(t));
		EndOfToken();
	}
	else if (c=='$') {
		search += c;

		// This might be a keyword ending in $ so check that first
		if (KeywordCheck(true))
			return;

		Token t = CreateToken();
		t.type = TokenType::IDENTIFIER_STRING;
		t.text = search;
		tokens.push_back(std::move(t));
		EndOfToken();
	}
	else if (c=='#') {
		search += c;

		// This might be a keyword ending in # so check that first
		if (KeywordCheck(true))
			return;

		Token t = CreateToken();
		t.type = TokenType::IDENTIFIER_FLOAT;
		t.text = search;
		tokens.push_back(std::move(t));
		EndOfToken();
	}
	else {
		if (c=='\n')
			NewLine();
		if (KeywordCheck(true, true)) {
			if (c!='\n')
				HandleCharacter(c);
			return;
		}
		Token t = CreateToken();
		t.type = TokenType::IDENTIFIER;
		t.text = search;
		tokens.push_back(std::move(t));
		EndOfToken();
		if (c!='\n')
			HandleCharacter(c);
	}
}

bool Tokeniser::KeywordCheck(bool complete, bool ignore)
{
	// If fresh search, clean the match list
	if (search.length()==1) {
		keyword_tree = &root;
	}
	else if (keyword_tree==nullptr) {
		return false;
	}

	// Scan and find
	if (!ignore) {
		char c = search.back();
		auto f = keyword_tree->leaves.find(c);
		if (f==keyword_tree->leaves.end()) {
			// Can't find
			keyword_tree = nullptr;
			return false;
		}
		keyword_tree = &f->second;
	}

	// Complete?
	if (complete) {
		if (keyword_tree->token.type!=TokenType::NONE) {
			CreateTokenAndAdd(keyword_tree->token.type);
			return true;
		}
	}

	// Do we have a match?
	if (keyword_tree->token.type!=TokenType::NONE && keyword_tree->leaves.empty()) {
		CreateTokenAndAdd(keyword_tree->token.type);
		return true;
	}

	return false;
}
