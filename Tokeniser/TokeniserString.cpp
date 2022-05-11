#include "Tokeniser.h"

void Tokeniser::HandleString(const char &c)
{
    if (c == '"')
    {
        // Check this a valid string
        if (search.length() > 0)
        {
            search.replace(0, 1, "");
            Token t = CreateToken();
            t.type = TokenType::LITERAL_STRING;
            t.text = search;
            tokens.push_back(std::move(t));
            EndOfToken();
        }
        else
        {
            search += c;
        }
    }
    else
    {
        search += c;
    }
}
