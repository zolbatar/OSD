#include "Parser.h"

void Parser::Parser_CASE(Token* t, std::list<Token*>* tokens_out)
{
	// Expression
	t->expressions.push_back(std::list<Token*>());
	ParseExpression(false, false, &t->expressions[0]);

	// Space for temp variable, we will build it in the IR stage as we don't know the type yet
	t->index = local_index++;

	// Now expect an OF
	auto tt = GetToken();
	if (tt->type!=TokenType::OF)
		Error("OF expected", tt);

	// Loop through all WHENs
	while (1) {

		// Ignore new lines
		tt = GetToken();
		while (tt->type==TokenType::NEWLINE)
			tt = GetToken();

		if (tt->type==TokenType::WHEN) {
			// Process each expression choice
			int option = 0;
			while (1) {

				// Loop through and grab all possible expressions to match against
				tt->expressions.push_back(std::list<Token*>());
				ParseExpression(false, false, &tt->expressions[option++]);

				auto tt = GetToken();
				if (tt->type==TokenType::COLON)
					break;
				else if (tt->type==TokenType::COMMA) {
					// All good
				}
				else
					SyntaxError(tt);
			}

			// Parse statements
			ParseSequenceOfStatements(tt, { TokenType::WHEN, TokenType::OTHERWISE, TokenType::ENDCASE });
			t->stack.push_back(tt);
		}
		else if (tt->type==TokenType::OTHERWISE) {
			// Get rid of any newlines
			tt = GetToken();
			while (tt->type==TokenType::NEWLINE)
				tt = GetToken();
			PushTokenBack();
			ParseSequenceOfStatements(t, { TokenType::ENDCASE });
		}
		else if (tt->type==TokenType::ENDCASE) {
			// We are done
			break;
		}
		else
			Error("WHEN, OTHERWISE or ENDCASE expected", tt);

	}
	tokens_out->push_back(t);
}