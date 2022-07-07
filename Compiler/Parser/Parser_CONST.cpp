#include "Parser.h"

void Parser::Parser_CONST(Token* t, std::list<Token*>* tokens_out)
{
	while (true) {
		// Get next token, which should be the variable name
		auto tt = GetToken();
		auto saved_type = tt->type;
		switch (tt->type) {
			case TokenType::IDENTIFIER_INTEGER:
				tt->vtype = TypeInteger();
				break;
			case TokenType::IDENTIFIER_FLOAT:
				tt->vtype = TypeFloat();
				break;
			case TokenType::IDENTIFIER_STRING:
				tt->vtype = TypeString();
				break;
			default:
				Error("Identifier expected", tt);
		}
		tt->type = TokenType::CONST;
		tt->name = std::move(tt->text);

		// Next should be an equals
		auto tt2 = GetToken();
		if (tt2->type!=TokenType::EQUAL)
			SyntaxError(tt);

		// And finally the value
		tt2 = GetToken();
		switch (saved_type) {
			case TokenType::IDENTIFIER_INTEGER:
				if (tt2->type==TokenType::LITERAL_FLOAT)
					assert(false); // Conversion needed
				else if (tt2->type!=TokenType::LITERAL_INTEGER)
					Error("Literal value of wrong type", tt);
				tt->integer = tt2->integer;
				break;
			case TokenType::IDENTIFIER_FLOAT:
				if (tt2->type==TokenType::LITERAL_INTEGER)
					assert(false); // Conversion needed
				else if (tt2->type!=TokenType::LITERAL_FLOAT)
					Error("Literal value of wrong type", tt);
				tt->real = tt2->real;
				break;
			case TokenType::IDENTIFIER_STRING:
				if (tt2->type!=TokenType::LITERAL_STRING)
					Error("Literal value of wrong type", tt);
				tt->text = tt2->text;
				break;
			default:
				Error("Identifier expected", tt);
		}

		// Unique name?
		if (constants.find(tt->name)==constants.end()) {
			constants.insert(tt->name);
		}
		else {
			Error("Constant not unique", tt);
		}
		tokens_out->push_back(tt);

		// Do we have a comma?
		tt = GetToken();
		if (tt->type==TokenType::COLON || tt->type==TokenType::NEWLINE) {
			return;
		}
		else if (tt->type==TokenType::COMMA) {
			// Do nothing, loop around and see if there is another
		}
	}
}
