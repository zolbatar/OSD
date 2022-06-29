#include "IRCompiler.h"

void IRCompiler::CompileTokenLiteral(Token *token)
{
    switch (token->type)
    {
    case TokenType::LITERAL_INTEGER:
        AddIRIntegerLiteral(token, token->integer);
        type_stack.push(ValueType::Integer);
        break;
    case TokenType::LITERAL_FLOAT:
        AddIRFloatLiteral(token, token->real);
        type_stack.push(ValueType::Float);
        break;
    case TokenType::LITERAL_STRING:
        AddIRStringLiteral(token, token->text);
        type_stack.push(ValueType::String);
        break;
    default:
        Error(token, "Why?");
    }
}
