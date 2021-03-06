#include "Breakdown.h"

std::map<size_t, LineBreakdown> Breakdown::line_breakdown;
std::map<size_t, std::string> Breakdown::native;

void Breakdown::InsertLineMapping(size_t line, jit_node_t *node)
{
    LineMapping lm;
    lm.node = node;
    lm.address = 0;
    CheckLineExists(line);
    auto f = line_breakdown.find(line);
    f->second.native.push_back(std::move(lm));
}

void Breakdown::InsertLineMappingGlobal(size_t line, jit_node_t *node)
{
    LineMapping lm;
    lm.node = node;
    lm.address = 0;
    CheckLineExists(line);
    auto f = line_breakdown.find(line);
    f->second.native_global.push_back(std::move(lm));
}

void Breakdown::ProcessLineMappings(jit_state_t *_jit)
{
    for (auto &l : line_breakdown)
    {
        for (auto &l1 : l.second.native)
        {
            l1.address = (size_t)jit_address(l1.node);
            // CLogger::Get()->Write("NativeCompiler", LogNotice, "%d = %x", l.first, l1.address);
        }
        for (auto &l1 : l.second.native_global)
        {
            l1.address = (size_t)jit_address(l1.node);
            // CLogger::Get()->Write("NativeCompiler", LogNotice, "%d = %x", l.first, l1.address);
        }
    }
}

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
        if (OSDTask::yield_due)
            GetCurrentTask()->Yield();

        std::string h1("Line: " + std::to_string(l.first) + "\n");
        f_puts(h1.c_str(), &fil);
        f_puts("* Raw tokens:\n", &fil);
        for (auto &l1 : l.second.tokens)
        {
            f_puts("    ", &fil);
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        f_puts("** Parsed tokens:\n", &fil);
        for (auto &l1 : l.second.parser)
        {
            f_puts("    ", &fil);
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        f_puts("*** IR:\n", &fil);
        for (auto &l1 : l.second.IR)
        {
            f_puts("    ", &fil);
            f_puts(l1.c_str(), &fil);
            f_puts("\n", &fil);
        }
        /*        f_puts("**** Native:\n", &fil);
                for (auto &l1 : l.second.native)
                {
                    f_puts("    ", &fil);
                    f_puts(l1.c_str(), &fil);
                    f_puts("\n", &fil);
                }
                f_puts("----\n", &fil);*/
    }

    // Now output all lines
    for (auto &l : native)
    {
        f_puts("    ", &fil);
        f_puts(l.second.c_str(), &fil);
        f_puts("\n", &fil);
    }

    f_close(&fil);
}

void Breakdown::ProcessTokeniser(Tokeniser *t)
{
    t->PrintTokens(t->Tokens(), 0, true);
}

void Breakdown::ProcessParser(Tokeniser *t, Parser *p)
{
    t->PrintTokensPtr(p->Tokens(), 0, false);
}

void Breakdown::ProcessIR(IRCompiler *ir)
{
    ir->IRPrinter();
}

void Breakdown::ProcessNative(NativeCompiler *nc)
{
    nc->Disassemble(&native);
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

std::string blank = "BLANK";

LineBreakdown *Breakdown::GetLineBreakdown(size_t line_number)
{
    auto f = line_breakdown.find(line_number);
    if (f == line_breakdown.end())
    {
        return NULL;
    }
    return &f->second;
}

std::string *Breakdown::GetNativeForAddress(size_t address)
{
    auto f = native.find(address);
    if (f == native.end())
        return &blank;
    return &f->second;
}
