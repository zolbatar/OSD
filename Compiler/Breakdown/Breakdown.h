#pragma once
#include <Compiler.h>

extern "C"
{
#include "../Compiler/Lightning/lightning.h"
#include "../Compiler/Lightning/jit_private.h"
}

struct LineMapping
{
    jit_node_t *node;
    size_t address;
};

struct LineBreakdown
{
    std::string source;
    std::list<std::string> tokens;
    std::list<std::string> parser;
    std::list<std::string> IR;
    std::list<LineMapping> native_global;
    std::list<LineMapping> native;
};

class Breakdown
{
  public:
    static void Init()
    {
        line_breakdown.clear();
        native.clear();
    }
    static LineBreakdown *GetLineBreakdown(size_t line_number);
    static std::list<std::string> *GetRowTokens(size_t line);
    static std::list<std::string> *GetRowParser(size_t line);
    static std::list<std::string> *GetRowIR(size_t line);
    static void InsertLineMapping(size_t line, jit_node_t *node);
    static void InsertLineMappingGlobal(size_t line, jit_node_t *node);
    static void ProcessLineMappings(jit_state_t *_jit);
    static void ProcessTokeniser(Tokeniser *t);
    static void ProcessParser(Tokeniser *t, Parser *p);
    static void ProcessIR(IRCompiler *ir);
    static void ProcessNative(NativeCompiler *nc);
    static void Output(FileSystem *fs, std::string volume, std::string directory, std::string filename);
    static std::string *GetNativeForAddress(size_t address);

  private:
    static void CheckLineExists(size_t line);

    static std::map<size_t, LineBreakdown> line_breakdown;
    static std::map<size_t, std::string> native;
};
