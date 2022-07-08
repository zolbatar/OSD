#pragma once
#include <OS_Includes.h>
#include <Exception/DARICException.h>
#include "Tokens.h"

enum class TokeniserState
{
    NONE,
    NUMBER,
    TEXT,
    STRING,
    SYMBOL,
    REM
};

struct TokenDef
{
    std::string name;
    TokenType type;
};

class Leaf
{
  public:
    TokenDef token{"", TokenType::NONE};
    std::map<char, Leaf> leaves;
};

struct Token
{
    // Type
    TokenType type = TokenType::NONE;
    ComplexType vtype = TypeNone();
    int64_t index = -1;
    int64_t index2;
    int64_t index3;

    // Name (for variables etc)
    std::string name;

    // Values
    std::string text;
    int64_t integer = 0;
    double real = 0.0;

    // Generic function
    void *func;

    // Position
    int line_number;
    int file_number = 0;
    int char_number;

    // Children
    std::list<ComplexType> return_types;
    std::list<ComplexType> required_types;
    std::list<Token *> stack;
    std::vector<std::list<Token *>> expressions;
    std::vector<std::list<Token *>> branches;
    void *IR;
};

class Tokeniser
{
  public:
    Tokeniser(std::string filename, std::string code) : filename(filename), code(code)
    {
        filenames.push_back(filename);
    }
    static void Init();
    void Parse();

    std::list<Token> *Tokens()
    {
        return &tokens;
    }

    std::vector<std::string> *GetFilenames()
    {
        return &filenames;
    }

    void PrintTokens(std::list<Token> *tokens, int depth, bool stage1);
    void PrintTokensPtr(std::list<Token *> *tokens, int depth, bool stage1);

  private:
    void PrintToken(Token *token, int depth, std::list<std::string> *output);
    static void AddKeyword(TokenDef def);
    void EndOfToken();
    void CreateTokenAndAdd(TokenType type);
    void Error(std::string error);
    void NewLine();
    bool KeywordCheck(bool complete, bool ignore = false);

    Token CreateToken()
    {
        Token t;
        t.line_number = line_number;
        t.char_number = token_char_number;
        t.file_number = file_number;
        return t;
    }

    // Handlers
    void HandleNone(const char &c);
    void HandleCharacter(const char &c);
    void HandleSymbol(const char &c);
    void HandleNumber(const char &c);
    void HandleText(const char &c);
    void HandleString(const char &c);

    std::string filename;
    std::string code;
    int file_number = 0;
    int char_number = 1;
    int token_char_number;
    int line_number = 1;
    std::string search;
    TokeniserState state = TokeniserState::NONE;
    std::list<Token> tokens;
    std::vector<std::string> filenames;
    Leaf *keyword_tree = &root;

    static Leaf root;
    static std::map<TokenType, TokenDef> keyword_lookup;
    static std::map<std::string, TokenType> symbols;
    static std::list<TokenDef> tokendefs;
};
