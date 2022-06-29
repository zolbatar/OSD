#pragma once
#include <list>
#include <map>
#include <string>
#include "../Tasks/System/FileManager/FileSystemObject.h"
#include "../Parser/Parser.h"
#include "../Tokeniser/Tokeniser.h"
#include "../IRCompiler/IRCompiler.h"
#include "../NativeCompiler/NativeCompiler.h"

struct LineBreakdown
{
    std::string source;
    std::list<std::string> tokens;
    std::list<std::string> parser;
    std::list<std::string> IR;
    std::list<std::string> native;
};

class Breakdown
{
  public:
    static void Init()
    {
        line_breakdown.clear();
    }
    static std::list<std::string> *GetRowTokens(size_t line);
    static std::list<std::string> *GetRowParser(size_t line);
    static std::list<std::string> *GetRowIR(size_t line);
    static std::list<std::string> *GetRowNative(size_t line);
    static void SetSource(size_t line, std::string source);
    static void ProcessTokeniser(Tokeniser *t);
    static void ProcessParser(Tokeniser *t, Parser *p);
    static void ProcessIR(IRCompiler *ir);
    static void ProcessNative(NativeCompiler *nc);
    static void Output(FileSystem *fs, std::string volume, std::string directory, std::string filename);

  private:
    static void CheckLineExists(size_t line);

    static std::map<size_t, LineBreakdown> line_breakdown;
    static std::list<std::string> __native;
};
