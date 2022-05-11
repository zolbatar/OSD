#include "Tokeniser.h"
#include <algorithm>
#include <iostream>

void Tokeniser::HandleText(const char &c) {
    if (std::isalnum(c) || c == '_') {
        search += c;
        if (KeywordCheck(false))
            return;
    } else if (c == '%') {
        search += c;

        // This might be a keyword ending in % so check that first
        if (KeywordCheck(true))
            return;

        Token t = CreateToken();
        t.type = TokenType::IDENTIFIER_INTEGER;
        t.text = search;
        tokens.push_back(std::move(t));
        EndOfToken();
    } else if (c == '!') {
        search += c;

        // This might be a keyword ending in % so check that first
        if (KeywordCheck(true))
            return;

        Token t = CreateToken();
        t.type = TokenType::IDENTIFIER_TYPE;
        t.text = search;
        tokens.push_back(std::move(t));
        EndOfToken();
    } else if (c == '$') {
        search += c;

        // This might be a keyword ending in $ so check that first
        if (KeywordCheck(true))
            return;

        Token t = CreateToken();
        t.type = TokenType::IDENTIFIER_STRING;
        t.text = search;
        tokens.push_back(std::move(t));
        EndOfToken();
    } else if (c == '#') {
        search += c;

        // This might be a keyword ending in # so check that first
        if (KeywordCheck(true))
            return;

        Token t = CreateToken();
        t.type = TokenType::IDENTIFIER_FLOAT;
        t.text = search;
        tokens.push_back(std::move(t));
        EndOfToken();
    } else {
        if (c == '\n')
            NewLine();
        if (KeywordCheck(true))
            return;
        Token t = CreateToken();
        t.type = TokenType::IDENTIFIER;
        t.text = search;
        tokens.push_back(std::move(t));
        EndOfToken();
        if (c != '\n')
            HandleCharacter(c);
    }
}

bool Tokeniser::KeywordCheck(bool complete) {
    // Create upper case version
    std::string upper_search = search;
    char c = upper_search[0];
//    std::transform(upper_search.begin(), upper_search.end(), upper_search.begin(), ::toupper);

    // If fresh search, clean the match list
    if (search.length() == 1) {
        current_match_list.clear();
        auto kw = keywords.find(c);
        if (kw != keywords.end()) {
            for (auto it = kw->second.begin(); it != kw->second.end(); ++it) {
                current_match_list.push_back(*it);
            }
        }
    }

    // If this is completed, do exact match
    if (complete) {
        for (auto it = current_match_list.begin(); it != current_match_list.end(); ++it) {
            if (upper_search.compare((*it).name) == 0) {
                CreateTokenAndAdd((*it).type);
                return true;
            }
        }
    } else {
        if (upper_search.length() > 1) {
            current_match_list.remove_if([&](const TokenDef &x) -> bool {
                return x.name.substr(0, upper_search.length()).compare(upper_search);
            });
        }

        // Do we have one match, if so, check it's an exact match
        if (current_match_list.size() == 1 && current_match_list.front().name.compare(upper_search) == 0) {
            CreateTokenAndAdd(current_match_list.front().type);
            return true;
        }
    }

    return false;
}
