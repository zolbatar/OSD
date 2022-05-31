#include "Tokeniser.h"

void Tokeniser::HandleNumber(const char& c)
{
	if (std::isdigit(c) || c=='.' || (c>='a' && c<='f') || (c>='A' && c<='F')) {
		search += c;
	}
	else if (c=='-' && search.length()==0) {
		search += c;
	}
	else if (c=='+' && search.length()==0) {
		search += c;
	}
	else if (search.length()==0 && c=='&') {
		search += c;
	}
	else if (search.length()==0 && c=='%') {
		search += c;
	}
	else {
		if (search.length()==1 && search[0]=='-') {
			// This is unary minus
			auto found = symbols.find("UNARY-");
			CreateTokenAndAdd((*found).second);
			HandleCharacter(c);
			return;
		}
		else if (search.length()==1 && search[0]=='+') {
			// This is unary plus
			auto found = symbols.find("UNARY+");
			CreateTokenAndAdd((*found).second);
			HandleCharacter(c);
			return;
		}
			// Figure out what type of number this is
		else if (search[0]=='&') {
			search.replace(0, 1, "");
			uint64_t i;
			try {
				i = std::stoll(search, 0, 16);
			}
			catch (const std::invalid_argument&) {
				Error("Syntax error");
			}

			Token t = CreateToken();
			t.type = TokenType::LITERAL_INTEGER;
			t.integer = i;
			tokens.push_back(std::move(t));
		}
		else if (search[0]=='%') {
			search.replace(0, 1, "");
			uint64_t i = std::stoll(search, 0, 2);
			Token t = CreateToken();
			t.type = TokenType::LITERAL_INTEGER;
			t.integer = i;
			tokens.push_back(std::move(t));
		}
		else if (search.find('.')!=std::string::npos) {
			double f = std::stod(search);
			Token t = CreateToken();
			t.type = TokenType::LITERAL_FLOAT;
			t.real = f;
			tokens.push_back(std::move(t));
		}
		else {
			uint64_t i = std::stoll(search);

			// Could this be a line number?
			if (tokens.size()==0 || tokens.back().type==TokenType::NEWLINE) {
				line_number = i;
				Token t = CreateToken();
				t.type = TokenType::LINE_NUMBER;
				t.integer = i;
				tokens.push_back(std::move(t));
			}
			else {
				Token t = CreateToken();
				t.type = TokenType::LITERAL_INTEGER;
				t.integer = i;
				tokens.push_back(std::move(t));
			}
		}
		EndOfToken();
		HandleCharacter(c);
	}
}
