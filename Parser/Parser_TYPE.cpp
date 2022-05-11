#include "Parser.h"

void Parser::Parser_TYPE(Token* t, std::list<Token*>* tokens_out)
{
	// Get name
	auto tt = GetToken();
	if (types.count(tt->text)>0)
		Error("This type isn't unique", tt);

	// List of fields, separated by commas
	Type typ;
	typ.name = std::move(tt->text);
	size_t position = 0;
	while (1) {
		TypeField tf;
		auto tt = GetToken();
		switch (tt->type) {
			case TokenType::IDENTIFIER:
				Error("Field must have type", tt);
			case TokenType::IDENTIFIER_INTEGER:
				tf.name = std::move(tt->text);
				tf.type = ValueType::Integer;
				tf.position = position;
				position+=sizeof(int64_t);
				break;
			case TokenType::IDENTIFIER_FLOAT:
				tf.name = std::move(tt->text);
				tf.type = ValueType::Float;
				tf.position = position;
				position+=sizeof(double);
				break;
			case TokenType::IDENTIFIER_STRING:
				tf.name = std::move(tt->text);
				tf.type = ValueType::String;
				tf.position = position;
				position+=sizeof(int64_t);
				break;
		}
        typ.field_names.push_back(tf.name);
		typ.fields.insert(std::make_pair(tf.name, std::move(tf)));

		// Comma or newline both fine?
		tt = GetToken();
		if (tt->type==TokenType::COMMA) {
			// Fine
		}
		else if (tt->type==TokenType::NEWLINE) {
			// Clear all new lines
			do {
				tt = GetToken();
			}
			while (tt->type==TokenType::NEWLINE);
			PushTokenBack();
		}
		else if (tt->type==TokenType::ENDTYPE) {
			break;
		}
		else
			SyntaxError(tt);

		tt = GetToken();
		if (tt->type==TokenType::ENDTYPE)
			break;
		PushTokenBack();
	}
	typ.size = position;
	types.insert(std::make_pair(typ.name, std::move(typ)));
}
