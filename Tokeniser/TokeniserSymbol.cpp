#include "Tokeniser.h"

void Tokeniser::HandleSymbol(const char& c)
{
	/*if ((c=='-' && search.length()==1 && search[0]=='-') ||
			(c=='-' && search.length()==1 && search[0]=='+') ||
			(c=='+' && search.length()==1 && search[0]=='-') ||
			(c=='+' && search.length()==1 && search[0]=='+')) {
		// Binary followed by unary minus
		auto found = symbols.find(search);
		CreateTokenAndAdd((*found).second);
		HandleCharacter(c);
	}
	else */
	if (std::isalnum(c) || c==' ' || c=='\n' || c==':') {
		// If this is alphanumeric it's probably end of token
		auto found = symbols.find(search);
		if (found!=symbols.end()) {
			CreateTokenAndAdd((*found).second);
			HandleCharacter(c);
			return;
		}
		Error("Syntax error");
	}
	else {
		auto previous = search;
		auto found = symbols.find(search);
		search += c;
		auto found_new = symbols.find(search);

		// New larger symbol invalid?
		if (found_new==symbols.end()) {
			if (found==symbols.end()) {
				Error("Syntax error");
			}
			else {
				CreateTokenAndAdd((*found).second);
				HandleCharacter(c);
			}
		}
	}
}
