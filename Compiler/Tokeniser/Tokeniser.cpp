#include "Tokeniser.h"
#include <algorithm>
#include <iterator>
#include "../Exception/DARICException.h"

Leaf Tokeniser::root;
std::map<TokenType, TokenDef> Tokeniser::keyword_lookup;
std::map<std::string, TokenType> Tokeniser::symbols;

void Tokeniser::Init()
{
	// Symbols
	symbols.insert(std::make_pair("=", TokenType::EQUAL));
	symbols.insert(std::make_pair("~", TokenType::TILDE));
	symbols.insert(std::make_pair(",", TokenType::COMMA));
	symbols.insert(std::make_pair("(", TokenType::LPARENS));
	symbols.insert(std::make_pair(")", TokenType::RPARENS));
	symbols.insert(std::make_pair(";", TokenType::SEMICOLON));
	symbols.insert(std::make_pair("!", TokenType::BANG));
	symbols.insert(std::make_pair(":", TokenType::COLON));
	symbols.insert(std::make_pair("^", TokenType::HAT));
	symbols.insert(std::make_pair("'", TokenType::TICK));
	symbols.insert(std::make_pair("=", TokenType::EQUAL));
	symbols.insert(std::make_pair("~", TokenType::TILDE));
	symbols.insert(std::make_pair(",", TokenType::COMMA));
	symbols.insert(std::make_pair("(", TokenType::LPARENS));
	symbols.insert(std::make_pair(")", TokenType::RPARENS));
	symbols.insert(std::make_pair(";", TokenType::SEMICOLON));
	symbols.insert(std::make_pair("!", TokenType::BANG));
	symbols.insert(std::make_pair("<", TokenType::LESS));
	symbols.insert(std::make_pair(">", TokenType::GREATER));
	symbols.insert(std::make_pair("<=", TokenType::LESSEQUAL));
	symbols.insert(std::make_pair(">=", TokenType::GREATEREQUAL));
	symbols.insert(std::make_pair("<>", TokenType::NOTEQUAL));
	symbols.insert(std::make_pair("+", TokenType::ADD));
	symbols.insert(std::make_pair("+=", TokenType::ADD_EQUAL));
	symbols.insert(std::make_pair("-", TokenType::SUBTRACT));
	symbols.insert(std::make_pair("UNARY-", TokenType::UNARYMINUS));
	symbols.insert(std::make_pair("UNARY+", TokenType::UNARYPLUS));
	symbols.insert(std::make_pair("-=", TokenType::SUBTRACT_EQUAL));
	symbols.insert(std::make_pair("*", TokenType::MULTIPLY));
	symbols.insert(std::make_pair("*=", TokenType::MULTIPLY_EQUAL));
	symbols.insert(std::make_pair("/", TokenType::DIVIDE));
	symbols.insert(std::make_pair("//", TokenType::DIVIDE_INTEGER));
	symbols.insert(std::make_pair("<<", TokenType::SHIFT_LEFT));
	symbols.insert(std::make_pair(">>", TokenType::SHIFT_RIGHT));
	symbols.insert(std::make_pair("<<=", TokenType::SHIFT_LEFT_EQUAL));
	symbols.insert(std::make_pair(">>=", TokenType::SHIFT_RIGHT_EQUAL));

	// Add symbols to reverse lookup
	for (auto& s: symbols) {
		TokenDef def;
		def.type = s.second;
		def.name = s.first;
		keyword_lookup.insert(std::make_pair(def.type, def));
	}

	// Standard
	AddKeyword(TokenDef{ "CASE", TokenType::CASE });
	AddKeyword(TokenDef{ "CONST", TokenType::CONST });
	AddKeyword(TokenDef{ "DATA", TokenType::DATA });
	AddKeyword(TokenDef{ "DATALABEL", TokenType::DATALABEL });
	AddKeyword(TokenDef{ "DEF", TokenType::DEF });
	AddKeyword(TokenDef{ "DIM", TokenType::DIM });
	AddKeyword(TokenDef{ "ELSE", TokenType::ELSE });
	AddKeyword(TokenDef{ "END", TokenType::END });
	AddKeyword(TokenDef{ "ENDCASE", TokenType::ENDCASE });
	AddKeyword(TokenDef{ "ENDIF", TokenType::ENDIF });
	AddKeyword(TokenDef{ "ENDDEF", TokenType::ENDDEF });
	AddKeyword(TokenDef{ "ENDTYPE", TokenType::ENDTYPE });
	AddKeyword(TokenDef{ "ENDWHILE", TokenType::ENDWHILE });
	AddKeyword(TokenDef{ "FOR", TokenType::FOR });
	AddKeyword(TokenDef{ "GLOBAL", TokenType::GLOBAL });
//	AddKeyword(TokenDef{ "GOSUB", TokenType::GOSUB });
//	AddKeyword(TokenDef{ "GOTO", TokenType::GOTO });
	AddKeyword(TokenDef{ "INKEY", TokenType::INKEY });
	AddKeyword(TokenDef{ "INKEY$", TokenType::INKEYS });
	AddKeyword(TokenDef{ "INPUT", TokenType::INPUT });
	AddKeyword(TokenDef{ "IF", TokenType::IF });
	AddKeyword(TokenDef{ "LET", TokenType::LET });
	AddKeyword(TokenDef{ "LOCAL", TokenType::LOCAL });
	AddKeyword(TokenDef{ "NEXT", TokenType::NEXT });
	AddKeyword(TokenDef{ "OF", TokenType::OF });
	AddKeyword(TokenDef{ "OTHERWISE", TokenType::OTHERWISE });
	AddKeyword(TokenDef{ "PRINT", TokenType::PRINT });
	AddKeyword(TokenDef{ "READ", TokenType::READ });
	AddKeyword(TokenDef{ "REM", TokenType::REM });
	AddKeyword(TokenDef{ "REPEAT", TokenType::REPEAT });
	AddKeyword(TokenDef{ "RESTORE", TokenType::RESTORE });
	AddKeyword(TokenDef{ "RETURN", TokenType::RETURN });
	AddKeyword(TokenDef{ "SPC", TokenType::SPC });
	AddKeyword(TokenDef{ "STEP", TokenType::STEP });
	AddKeyword(TokenDef{ "SWAP", TokenType::SWAP });
	AddKeyword(TokenDef{ "TAB", TokenType::TAB });
	AddKeyword(TokenDef{ "THEN", TokenType::THEN });
	AddKeyword(TokenDef{ "TO", TokenType::TO });
	AddKeyword(TokenDef{ "TIME", TokenType::TIME });
	AddKeyword(TokenDef{ "TIME$", TokenType::TIMES });
	AddKeyword(TokenDef{ "TYPE", TokenType::TYPE });
	AddKeyword(TokenDef{ "UNTIL", TokenType::UNTIL });
	AddKeyword(TokenDef{ "WHEN", TokenType::WHEN });
	AddKeyword(TokenDef{ "WHILE", TokenType::WHILE });
	AddKeyword(TokenDef{ "YIELD", TokenType::YIELD });

	// File
/*	AddKeyword(TokenDef{ "BGET#", TokenType::BGET });
	AddKeyword(TokenDef{ "BPUT#", TokenType::BPUT });
	AddKeyword(TokenDef{ "CLOSE#", TokenType::CLOSE });
	AddKeyword(TokenDef{ "EOF#", TokenType::EOFH });
	AddKeyword(TokenDef{ "OPENIN", TokenType::OPENIN });
	AddKeyword(TokenDef{ "OPENOUT", TokenType::OPENOUT });
	AddKeyword(TokenDef{ "OPENUP", TokenType::OPENUP });
	AddKeyword(TokenDef{ "GET$#", TokenType::GETSH });*/

	// String
	AddKeyword(TokenDef{ "ASC", TokenType::ASC });
	AddKeyword(TokenDef{ "CHR$", TokenType::CHRS });
	AddKeyword(TokenDef{ "INSTR", TokenType::INSTR });
	AddKeyword(TokenDef{ "LEFT$", TokenType::LEFTS });
	AddKeyword(TokenDef{ "MID$", TokenType::MIDS });
	AddKeyword(TokenDef{ "RIGHT$", TokenType::RIGHTS });
	AddKeyword(TokenDef{ "LEN", TokenType::LEN });
	AddKeyword(TokenDef{ "STR$", TokenType::STRS });
	AddKeyword(TokenDef{ "STRING$", TokenType::STRINGS });

	// Maths
	AddKeyword(TokenDef{ "ABS", TokenType::ABS });
	AddKeyword(TokenDef{ "FALSE", TokenType::_FALSE });
	AddKeyword(TokenDef{ "TRUE", TokenType::_TRUE });
	AddKeyword(TokenDef{ "INT", TokenType::INT });
	AddKeyword(TokenDef{ "FLOAT", TokenType::FLOAT });
	AddKeyword(TokenDef{ "DIV", TokenType::DIV });
	AddKeyword(TokenDef{ "MOD", TokenType::MOD });
	AddKeyword(TokenDef{ "SQR", TokenType::SQR });
	AddKeyword(TokenDef{ "LN", TokenType::LN });
	AddKeyword(TokenDef{ "LOG", TokenType::LOG });
	AddKeyword(TokenDef{ "EXP", TokenType::EXP });
	AddKeyword(TokenDef{ "ATN", TokenType::ATN });
	AddKeyword(TokenDef{ "TAN", TokenType::TAN });
	AddKeyword(TokenDef{ "COS", TokenType::COS });
	AddKeyword(TokenDef{ "SIN", TokenType::SIN });
	AddKeyword(TokenDef{ "ASN", TokenType::ASN });
	AddKeyword(TokenDef{ "ACS", TokenType::ACS });
	AddKeyword(TokenDef{ "DEG", TokenType::DEG });
	AddKeyword(TokenDef{ "RAD", TokenType::RAD });
	AddKeyword(TokenDef{ "SGN", TokenType::SGN });
	AddKeyword(TokenDef{ "VAL", TokenType::VAL });
	AddKeyword(TokenDef{ "PI", TokenType::PI });

	// Random
	AddKeyword(TokenDef{ "RND", TokenType::RND });
	AddKeyword(TokenDef{ "RNDF", TokenType::RNDF });

	// 2D Graphics
	AddKeyword(TokenDef{ "CIRCLE", TokenType::CIRCLE });
	AddKeyword(TokenDef{ "CIRCLEFILLED", TokenType::CIRCLEFILLED });
	AddKeyword(TokenDef{ "CLIPON", TokenType::CLIPON });
	AddKeyword(TokenDef{ "CLIPOFF", TokenType::CLIPOFF });
	AddKeyword(TokenDef{ "CLS", TokenType::CLS });
	AddKeyword(TokenDef{ "COLOUR", TokenType::COLOUR });
	AddKeyword(TokenDef{ "COLOURBG", TokenType::COLOURBG });
	AddKeyword(TokenDef{ "FLIP", TokenType::FLIP });
	AddKeyword(TokenDef{ "FONT", TokenType::SETFONT });
	AddKeyword(TokenDef{ "LINE", TokenType::LINE });
	AddKeyword(TokenDef{ "MODE", TokenType::MODE });
	AddKeyword(TokenDef{ "PLOT", TokenType::PLOT });
	AddKeyword(TokenDef{ "RECTANGLE", TokenType::RECTANGLE });
	AddKeyword(TokenDef{ "RECTANGLEFILLED", TokenType::RECTANGLEFILLED });
	AddKeyword(TokenDef{ "SCREENWIDTH", TokenType::SCREENWIDTH });
	AddKeyword(TokenDef{ "SCREENHEIGHT", TokenType::SCREENHEIGHT });
	AddKeyword(TokenDef{ "SHADOW", TokenType::SHADOW });
	AddKeyword(TokenDef{ "TEXT", TokenType::TEXT });
	AddKeyword(TokenDef{ "TEXTCENTRE", TokenType::TEXTCENTRE });
	AddKeyword(TokenDef{ "TEXTCENTER", TokenType::TEXTCENTRE });
	AddKeyword(TokenDef{ "TEXTRIGHT", TokenType::TEXTRIGHT });
	AddKeyword(TokenDef{ "TRIANGLE", TokenType::TRIANGLE });
	AddKeyword(TokenDef{ "TRIANGLEFILLED", TokenType::TRIANGLEFILLED });

	// 3d Graphics
/*	AddKeyword(TokenDef{ "VERTEX", TokenType::VERTEX3D });
	AddKeyword(TokenDef{ "FACE", TokenType::TRIANGLE3D });
	AddKeyword(TokenDef{ "SHAPE", TokenType::SHAPE3D });
	AddKeyword(TokenDef{ "OBJECT", TokenType::OBJECT3D });
	AddKeyword(TokenDef{ "SOLID", TokenType::SOLID });
	AddKeyword(TokenDef{ "WIREFRAME", TokenType::WIREFRAME });
	AddKeyword(TokenDef{ "EDGEDWIREFRAME", TokenType::EDGEDWIREFRAME });
	AddKeyword(TokenDef{ "TRANSLATE", TokenType::TRANSLATE3D });
	AddKeyword(TokenDef{ "ROTATE", TokenType::ROTATE3D });
	AddKeyword(TokenDef{ "SCALE", TokenType::SCALE3D });
	AddKeyword(TokenDef{ "RENDER", TokenType::RENDER3D });*/

	// Boolean
	AddKeyword(TokenDef{ "AND", TokenType::AND });
	AddKeyword(TokenDef{ "OR", TokenType::OR });
	AddKeyword(TokenDef{ "EOR", TokenType::EOR });
	AddKeyword(TokenDef{ "NOT", TokenType::NOT });
}

void Tokeniser::AddKeyword(TokenDef def)
{
	// Start at the top
	Leaf* start = &root;
	auto name = def.name;
	while (name.length()>0) {
		auto c = name[0];

		// Key?
		auto f = start->leaves.find(c);
		if (f==start->leaves.end()) {
			start->leaves.insert(std::make_pair(c, Leaf()));
			f = start->leaves.find(c);
		}

		// Next level
		start = &f->second;

		// Remainder
		name = name.substr(1);
	}

	// Add to end
	start->token = def;
	keyword_lookup.insert(std::make_pair(def.type, std::move(def)));
}

void Tokeniser::Parse()
{
	file_number = 0;
	line_number = 1;
	char_number = 1;
	search = "";
	state = TokeniserState::NONE;
	tokens.clear();
	keyword_tree = &root;

	// So, we simply keep reading characters and let the tokeniser convert these into tokens
	for (char c: code) {
		HandleCharacter(c);
		char_number++;
	}

	// This forces any remaining search to be processed
	HandleCharacter('\n');

	// End token
	Token t;
	t.type = TokenType::NONE;
	t.line_number = line_number-1;
	t.char_number = token_char_number;
	tokens.push_back(std::move(t));
}

void Tokeniser::HandleCharacter(const char& c)
{
	switch (state) {
		case TokeniserState::NONE:
			HandleNone(c);
			break;
		case TokeniserState::TEXT:
			HandleText(c);
			break;
		case TokeniserState::STRING:
			HandleString(c);
			break;
		case TokeniserState::NUMBER:
			HandleNumber(c);
			break;
		case TokeniserState::SYMBOL:
			HandleSymbol(c);
			break;
		case TokeniserState::REM:
			if (c=='\n') {
				state = TokeniserState::NONE;
				NewLine();
			}
			break;
		default:
			break;
	}
}

void Tokeniser::HandleNone(const char& c)
{
	if (std::isdigit(c) || c=='%' || c=='&' || c=='-' || c=='+') {

		// Is this unary or not?
		if (c=='-' || c=='+') {
			if (tokens.size()>0) {
				auto prev = tokens.back();
				switch (prev.type) {
					case TokenType::STEP:
					case TokenType::TO:
					case TokenType::EQUAL:
					case TokenType::LESS:
					case TokenType::GREATER:
					case TokenType::ADD:
					case TokenType::SUBTRACT:
					case TokenType::MULTIPLY:
					case TokenType::DIVIDE:
					case TokenType::COLON:
					case TokenType::SEMICOLON:
					case TokenType::HAT:
					case TokenType::LPARENS:
					case TokenType::NOTEQUAL:
					case TokenType::LESSEQUAL:
					case TokenType::GREATEREQUAL:
					case TokenType::ADD_EQUAL:
					case TokenType::SUBTRACT_EQUAL:
					case TokenType::MULTIPLY_EQUAL:
					case TokenType::DIVIDE_INTEGER:
					case TokenType::SHIFT_LEFT:
					case TokenType::SHIFT_RIGHT:
						break;
					default:
						state = TokeniserState::SYMBOL;
						token_char_number = char_number;
						HandleSymbol(c);
						return;
				}
			}
		}
		state = TokeniserState::NUMBER;
		token_char_number = char_number;
		HandleNumber(c);
	}
	else if (std::isalpha(c)) {
		state = TokeniserState::TEXT;
		token_char_number = char_number;
		HandleText(c);
	}
	else if (c=='"') {
		state = TokeniserState::STRING;
		token_char_number = char_number;
		HandleString(c);
	}
	else if (c==' ' || c=='\t') {
		// Swallow
	}
	else if (c=='\n') {
		Token t;
		t.line_number = line_number;
		t.char_number = char_number;
		t.type = TokenType::NEWLINE;
		t.text = c;
		tokens.push_back(std::move(t));
		NewLine();
	}
	else if (c==':') {
		if (tokens.size()>0) {
			auto prev = tokens.back();
			if (prev.type!=TokenType::COLON) {
				Token t;
				t.line_number = line_number;
				t.char_number = char_number;
				t.type = TokenType::COLON;
				t.text = c;
				tokens.push_back(std::move(t));
			}
		}
	}
	else {
		// This must be a symbol of some kind, we'll error if we don't match later
		state = TokeniserState::SYMBOL;
		token_char_number = char_number;
		HandleSymbol(c);
	}
}

void Tokeniser::EndOfToken()
{
	state = TokeniserState::NONE;
	search = "";
}

void Tokeniser::CreateTokenAndAdd(TokenType type)
{
	Token t;
	t.line_number = line_number;
	t.char_number = token_char_number;
	t.type = type;
	t.name = search;
	tokens.push_back(std::move(t));
	EndOfToken();
	if (type==TokenType::REM) {
		state = TokeniserState::REM;
	}
}

void Tokeniser::NewLine()
{
	line_number++;
	char_number = 1;
}

void Tokeniser::Error(std::string error)
{
	throw DARICException(ExceptionType::TOKENISER, filenames[file_number], line_number, char_number, error);
}
