#include "Parser.h"

void Parser::Parser_WHILE(Token *t, std::list<Token *> *tokens_out) {

    // Condition
    t->expressions.push_back(std::list<Token *>());
    ParseExpression(false, false, &t->expressions[0]);

    // Create statement(s)
    ParseSequenceOfStatements(t, false);

    // Next up should be an ENDWHILE
    auto tt = GetToken();
    if (tt->type != TokenType::ENDWHILE)
        Error("ENDWHILE expected", tt);

    tokens_out->push_back(t);
}
