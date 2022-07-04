#pragma once

#include <list>
#include <string>
#include <tuple>
#include <stack>
#include <tuple>
#include <set>
#include "../Tokeniser/Tokeniser.h"
#include "../Tokeniser/Types.h"
#include "../NativeCompiler/Functions.h"

typedef std::list<ComplexType> fp;

class Parser
{
  public:
    static void Init();
    bool Parse(bool optimise, std::list<Token> *tokens, std::vector<std::string> *filenames);

    std::list<Token *> *Tokens()
    {
        return &final_tokens;
    }
    static void *GetAddressForFunc(TokenType tt);

  private:
    bool optimise;
    static void AddFuncTokenType(TokenType type, std::list<ComplexType> types, void *func);
    std::vector<std::string> *filenames;
    void Error(std::string message, Token *token);
    void SyntaxError(Token *token);
    void TypeError(Token *type);
    int64_t local_index;
    int64_t global_index = -1;
    bool inside_function = false;
    Token *current_def = nullptr;
    std::map<std::string, Token *> all_defs;
    std::map<std::string, Token *> global_variables;
    std::map<std::string, Token *> local_variables;
    std::map<std::string, Type> types;

    Token *GetToken();
    void PushTokenBack();

    // Expression (shunting yard)
    void EnsureNodeIsToken(Token *t, TokenType oper);
    bool IsLeftAssociative(TokenType type);
    int GetOperatorPrecedence(TokenType type);
    bool TokenHasParameters(TokenType type);
    bool IsExpressionTerminator(bool assignment, Token *t, bool last_is_operator);

    bool IsStatementTerminator(bool assignment, Token *t);
    bool IsBlockTerminator(bool assignment, Token *t, std::set<TokenType> block_terminator);
    void ParseSequenceOfStatements(Token *t, std::set<TokenType> block_terminator);

    void ParseStatement(Token *t, std::list<Token *> *tokens_out);
    void ParseExpression(bool assignment, bool subexpression, std::list<Token *> *stack);
    void ParserStatementGeneric(Token *t, std::list<Token *> *tokens_out, fp *parameters, bool expression);

    void ParserAssignment(Token *t, std::list<Token *> *tokens_out, bool local_override);
    void Parser_CASE(Token *t, std::list<Token *> *tokens_out);
    void Parser_CONST(Token *t, std::list<Token *> *tokens_out);
    void Parser_DATA(Token *t, std::list<Token *> *tokens_out);
    void Parser_DATALABEL(Token *t, std::list<Token *> *tokens_out);
    void Parser_DEF(Token *t, std::list<Token *> *tokens_out);
    void Parser_DEFFastScan(Token *t);
    void Parser_FOR(Token *t, std::list<Token *> *tokens_out);
    void Parser_IF(Token *t, std::list<Token *> *tokens_out);
    void Parser_NEXT(Token *t, std::list<Token *> *tokens_out);
    void Parser_PRINT(Token *t, std::list<Token *> *tokens_out);
    void Parser_PROCCall(Token *t, std::list<Token *> *tokens_out);
    void Parser_READ(Token *t, std::list<Token *> *tokens_out);
    void Parser_REPEAT(Token *t, std::list<Token *> *tokens_out);
    void Parser_RESTORE(Token *t, std::list<Token *> *tokens_out);
    void Parser_RETURN(Token *t, std::list<Token *> *tokens_out);
    void Parser_SWAP(Token *t, std::list<Token *> *tokens_out);
    void Parser_TYPE(Token *t, std::list<Token *> *tokens_out);
    void Parser_WHILE(Token *t, std::list<Token *> *tokens_out);

    // Fields and types
    void FindType(Token *t);
    void FindField(Token *t, Token *tt);
    void FindOrCreateVariable(Token *t, std::list<Token *> *tokens_out, bool local_override);

    // Locals
    void CreateLocalVariable(Token *tt, bool init);
    Token *CreateNewLocalVariableNoInit(Token *t, std::string name, ComplexType type);

    // Globals
    void CreateGlobalVariable(Token *tt, bool init);
    Token *CreateNewGlobalVariableNoInit(Token *t, std::string name, ComplexType type);

    Token CreateToken(Token *parent, TokenType t);

    std::list<Token>::iterator it;
    std::list<Token>::iterator previous;
    std::list<Token *> final_tokens;
    std::list<Token> new_tokens;
    std::set<std::string> constants;
    std::stack<Token *> for_loop_variables;
    static std::map<TokenType, std::list<ComplexType>> generic_functions;
    static std::map<TokenType, void *> generic_functions_ptr;
};
