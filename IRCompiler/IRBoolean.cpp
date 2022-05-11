#include "IRCompiler.h"

void IRCompiler::CompileTokenBoolean(Token* token)
{
	switch (token->type) {
		case TokenType::NOT:
			DemoteMathsSingle(token, IROpcodes::BooleanNot);
			break;
		case TokenType::AND:
			DemoteMaths(token, IROpcodes::BooleanAnd);
			break;
		case TokenType::OR:
			DemoteMaths(token, IROpcodes::BooleanOr);
			break;
		case TokenType::EOR:
			DemoteMaths(token, IROpcodes::BooleanEor);
			break;
		default:
			Error(token, "Why?");
	}
}
