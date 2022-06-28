#include <circle/logger.h>
#include "Breakdown.h"

std::map<size_t, LineBreakdown> Breakdown::line_breakdown;

void Breakdown::Output(FileSystem *fs, std::string volume, std::string directory, std::string filename)
{
    FIL fil;
    std::string file(fs->GetCurrentDirectory() + "Combined.txt");
    if (f_open(&fil, file.c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        CLogger::Get()->Write("FontManager", LogPanic, "Error creating tokeniser debug file");
    }

    // Now output all lines
    for (auto &l : line_breakdown)
    {
        std::string h("Line: " + std::to_string(l.first) + "\n");
        f_puts(h.c_str(), &fil);
        for (auto &l1 : l.second.tokens)
        {
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        for (auto &l1 : l.second.parser)
        {
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        for (auto &l1 : l.second.IR)
        {
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        for (auto &l1 : l.second.native)
        {
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        f_puts("\n", &fil);
    }

    f_close(&fil);
}

void Breakdown::ProcessTokeniser(Tokeniser *t)
{
    std::list<std::string> tokens;
    t->PrintTokens(t->Tokens(), 1, &tokens, true);
}

void Breakdown::ProcessParser(Tokeniser *t, Parser *p)
{
    std::list<std::string> tokens;
    t->PrintTokensPtr(p->Tokens(), 5, &tokens, false);
}

std::list<std::string> *Breakdown::GetRowTokens(size_t line)
{
    CheckLineExists(line);
    return &line_breakdown.find(line)->second.tokens;
}

std::list<std::string> *Breakdown::GetRowParser(size_t line)
{
    CheckLineExists(line);
    return &line_breakdown.find(line)->second.parser;
}

std::list<std::string> *Breakdown::GetRowNative(size_t line)
{
    CheckLineExists(line);
    return &line_breakdown.find(line)->second.native;
}

std::list<std::string> *Breakdown::GetRowIR(size_t line)
{
    CheckLineExists(line);
    return &line_breakdown.find(line)->second.IR;
}

void Breakdown::CheckLineExists(size_t line)
{
    if (line_breakdown.find(line) == line_breakdown.end())
    {
        LineBreakdown lb;
        line_breakdown.insert(std::make_pair(line, std::move(lb)));
    }
}
