#include "Tokeniser.h"

void Tokeniser::HandleSymbol(const char& c)
{
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
