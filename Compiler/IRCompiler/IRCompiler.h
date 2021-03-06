#pragma once
#include <OS_Includes.h>
#include <Tokeniser/Tokeniser.h>
#include "IROpcodes.h"

class IRInstruction
{
  public:
    IRInstruction(Token *token, IROpcodes type, int64_t index, int64_t iv, double rv, std::string sv, TokenType tt)
        : line_number(token->line_number), file_number(token->file_number), type(type), index(index), iv(iv), rv(rv),
          sv(sv), tt(tt){};
    int line_number;
    int file_number;
    IROpcodes type;
    int64_t index;
    int64_t index2; // Used by optimiser
    int64_t iv;
    double rv;
    std::string sv;
    TokenType tt;
};

class IRCompiler
{
  public:
    IRCompiler(bool optimise, std::vector<std::string> *filenames) : optimise(optimise), filenames(filenames)
    {
    }

    void Compile(std::list<Token *> *tokens);
    void IRPrinter();

    std::list<IRInstruction> *GetIRInstructions()
    {
        return &ir;
    }

    std::list<IRInstruction> *GetGlobalIRInstructions()
    {
        return &ir_global;
    }

  private:
    bool optimise;
    void RunOptimiser(std::list<IRInstruction> *_ir);
    void IRPrinterSection(std::list<IRInstruction> *ir);
    void IRPrintValueBased(char *l, IROpcodes o, IROpcodes in, IROpcodes fl, IROpcodes st, std::string fmt, ...);
    void ForwardLookups(Token *token);
    void CheckForForwardLookup(Token *token);
    void Error(Token *token, std::string error);
    void TypeError(Token *token);
    void CheckParamType(Token *token, ValueType type);
    void CheckParamTypeOnly(Token *token, ValueType type);
    void SetReturnType(Token *token, ValueType type);
    void CompileToken(Token *token);
    void CompileTokenAssignment(Token *token);
    void CompileTokenBoolean(Token *token);
    void CompileTokenCase(Token *token);
    void CompileTokenConversion(Token *token);
    void CompileTokenConst(Token *token);
    void CompileTokenConstant(Token *token);
    void CompileTokenData(Token *token);
    void CompileTokenDef(Token *token);
    void CompileTokenFor(Token *token);
    void CompileTokenGlobalVariable(Token *token);
    void CompileTokenGlobalNoInitVariable(Token *token);
    void CompileTokenIf(Token *token);
    void CompileTokenLiteral(Token *token);
    void CompileTokenLocalVariable(Token *token);
    void CompileTokenLocalNoInitVariable(Token *token);
    void CompileTokenNext(Token *token);
    void CompileTokenPrint(Token *token);
    void CompileTokenProcCall(Token *token, bool expression);
    void CompileTokenRepeat(Token *token);
    void CompileTokenReturn(Token *token);
    void CompileTokenString(Token *token);
    void CompileTokenSwap(Token *token);
    void CompileTokenVariable(Token *token);
    void CompileTokenWhile(Token *token);

    // Comparison
    void CompileTokenComparison(Token *token);
    void GenericComparison(Token *token, ValueType type, IROpcodes i, IROpcodes r, IROpcodes s);

    // Maths
    void CompileTokenMaths(Token *token);
    void DemoteMaths(Token *token, IROpcodes r);
    void PromoteMaths(Token *token, IROpcodes r);
    void DemoteMathsSingle(Token *token, IROpcodes r);
    void GenericMaths(Token *token, ValueType type, IROpcodes i, IROpcodes r);
    void PromoteMathsSingle(Token *token, IROpcodes r);
    ValueType TypePromotion(Token *token, bool promote);
    ValueType TypeDemotion(Token *token);

    // Front loading stuff for DEFs, typically setting up variables
    void Init_AddIRWithIndex(Token *token, IROpcodes type, int64_t index);

    void Init_AddIR(Token *token, IROpcodes type);
    void AddIR(Token *token, IROpcodes type);
    void AddIRWithIndex(Token *token, IROpcodes type, int64_t index);
    void AddIRWithIntegerLiteral(Token *token, IROpcodes type, int64_t v);
    void AddIRWithFloatLiteral(Token *token, IROpcodes type, double v);
    void AddIRWithStringLiteral(Token *token, IROpcodes type, std::string v);
    void AddIRWithTokenTypeAndStringLiteral(Token *token, IROpcodes type, std::string v, TokenType tt);
    void AddIRWithStringLiteralWithInteger(Token *token, IROpcodes type, std::string v, int64_t iv);
    void AddIRWithIndexAndInteger(Token *token, IROpcodes type, int64_t index, int64_t iv);
    void Init_AddIRWithIndexAndInteger(Token *token, IROpcodes type, int64_t index, int64_t iv);
    void Init_AddIRWithStringLiteralWithIntegerAndIndex(Token *token, IROpcodes type, std::string v, int64_t iv,
                                                        int64_t index);
    void AddIRWithStringLiteralWithIntegerAndIndex(Token *token, IROpcodes type, std::string v, int64_t iv,
                                                   int64_t index);
    void AddIRIntegerLiteral(Token *token, int64_t v);
    void AddIRFloatLiteral(Token *token, double v);
    void AddIRStringLiteral(Token *token, std::string v);

    void EnsureStackIsInteger(Token *token, ValueType type);
    void EnsureStackIsFloat(Token *token, ValueType type);

    std::stack<ValueType> type_stack;
    std::list<IRInstruction> ir_global;
    std::list<IRInstruction> ir_global_init;
    std::list<IRInstruction> ir;
    std::list<IRInstruction> current_def;
    std::list<IRInstruction> current_def_init;
    std::map<std::string, int64_t> proc_lookup;
    std::map<std::string, Token *> const_lookup;
    std::stack<size_t> for_loop_jumps;
    std::stack<Token *> for_loop_tokens;
    size_t jump_index = 0;
    bool is_global = true;
    int64_t proc_index = 0;
    std::vector<std::string> *filenames;

    ValueType PopType(Token *token)
    {
        if (type_stack.empty())
            Error(token, "Stack empty, expected value.");
        auto type = type_stack.top();
        type_stack.pop();
        return type;
    }
};
