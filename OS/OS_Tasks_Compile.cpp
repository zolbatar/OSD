#include <iostream>
#include <sstream>
#include <circle/logger.h>
#include <circle/timer.h>
#include "OS.h"
#include "OS_Tasks.h"
#include "../Chrono/Chrono.h"
#include "../Parser/Parser.h"
#include "../Tokeniser/Tokeniser.h"
#include "../IRCompiler/IRCompiler.h"
#include "../NativeCompiler/NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../GUI/Window/Window.h"
#include "../OS/Breakdown.h"

void OSDTask::RunCode(bool wait)
{
    this->wait = wait;
    t1 = GetClock();
    jit_clear_state();
    if (exec != NULL)
        exec();

    // We never return here
}

std::string OSDTask::LoadSource(std::string volume, std::string directory, std::string filename)
{
    fs.SetVolume(volume);
    fs.SetCurrentDirectory(directory);
    filename = fs.GetCurrentDirectory() + filename;

    FIL fil;
    if (f_open(&fil, (filename).c_str(), FA_READ | FA_OPEN_EXISTING) != FR_OK)
    {
        CLogger::Get()->Write("OSDTask", LogPanic, "Error opening source file '%s'", filename.c_str());
    }
    size_t sz = f_size(&fil);
    char *buffer = (char *)malloc(sz + 1);
    if (!buffer)
    {
        CLogger::Get()->Write("OSDTask", LogPanic, "Error allocating memory for source file '%s'", filename.c_str());
    }
    uint32_t l;
    if (f_read(&fil, buffer, sz, &l) != FR_OK)
    {
        CLogger::Get()->Write("OSDTask", LogPanic, "Error loading source file '%s'", filename.c_str());
    }
    f_close(&fil);
    buffer[sz] = 0;
    std::string s(buffer);
    free(buffer);
    return s;
}

bool OSDTask::CompileSource(std::string volume, std::string directory, std::string filename, std::string code,
                            bool debug)
{
    fs.SetVolume(volume);
    fs.SetCurrentDirectory(directory);
    filename = fs.GetCurrentDirectory() + filename;

    bool optimise = true;
    Tokeniser token(filename, code);
    Parser parser;
    double total_time_span = 0;

    // Tokens
    auto t1 = CTimer::GetClockTicks();
    token.Parse();
    auto t2 = CTimer::GetClockTicks();
    double time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    Breakdown::ProcessTokeniser(&token);
    if (debug)
    {
        FIL fil;
        std::string file(fs.GetCurrentDirectory() + "Tokeniser.txt");
        if (f_open(&fil, file.c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error creating tokeniser debug file");
        }

        // Output time taken
        std::string t("Tokeniser: %f millis\n\n", time_span);
        f_puts(t.c_str(), &fil);

        // Now output all lines
        std::list<std::string> tokens;
        token.PrintTokens(token.Tokens(), 0, &tokens, true);
        for (auto &s : tokens)
        {
            f_puts(s.c_str(), &fil);
            f_puts("\n", &fil);
        }

        f_close(&fil);
    }

    t1 = CTimer::GetClockTicks();
    parser.Parse(optimise, token.Tokens(), token.GetFilenames());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    CLogger::Get()->Write("OSDTask", LogNotice, "Parser: %f millis", time_span);
    Breakdown::ProcessParser(&token, &parser);
    if (debug)
    {
        FIL fil;
        std::string file(fs.GetCurrentDirectory() + "Parser.txt");
        if (f_open(&fil, file.c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error creating parser debug file");
        }

        // Output time taken
        std::string t("Parser: %f millis\n\n", time_span);
        f_puts(t.c_str(), &fil);

        // Now output all lines
        std::list<std::string> tokens;
        token.PrintTokensPtr(parser.Tokens(), 0, &tokens, false);
        for (auto &s : tokens)
        {
            f_puts(s.c_str(), &fil);
            f_puts("\n", &fil);
        }

        f_close(&fil);
    }

    // Compile
    t1 = CTimer::GetClockTicks();
    IRCompiler ir_compiler(optimise, token.GetFilenames());
    ir_compiler.Compile(parser.Tokens());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    CLogger::Get()->Write("OSDTask", LogNotice, "IR Compiler: %f millis", time_span);
    if (debug)
    {
        FIL fil;
        std::string file(fs.GetCurrentDirectory() + "IR.txt");
        if (f_open(&fil, file.c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error creating IR debug file");
        }

        // Output time taken
        std::string t("IR Compiler: %f millis\n\n", time_span);
        f_puts(t.c_str(), &fil);

        // Now output all lines
        std::list<std::string> ir;
        ir_compiler.IRPrinter(&ir);
        for (auto &s : ir)
        {
            f_puts(s.c_str(), &fil);
            f_puts("\n", &fil);
        }

        f_close(&fil);
    }

    // Native
    _jit = jit_new_state();
    t1 = CTimer::GetClockTicks();
    NativeCompiler native_compiler(optimise, _jit, this);
    native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    CLogger::Get()->Write("OSDTask", LogNotice, "Native Compiler: %f millis", time_span);
    if (debug)
    {
        FIL fil;
        std::string file(fs.GetCurrentDirectory() + "Native.txt");
        if (f_open(&fil, file.c_str(), FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
        {
            CLogger::Get()->Write("FontManager", LogPanic, "Error creating IR debug file");
        }

        // Output time taken
        std::string t("IR Compiler: %f millis\nTotal: %f millis\n\n", time_span, total_time_span);
        f_puts(t.c_str(), &fil);

        // Now output all lines
        std::list<std::string> disassm;
        native_compiler.Disassemble(&disassm);
        for (auto &s : disassm)
        {
            f_puts(s.c_str(), &fil);
            f_puts("\n", &fil);
        }

        f_close(&fil);
    }

    // Output new style debug
    Breakdown::Output(&this->fs, volume, directory, filename);

    return true;
}

void OSDTask::SetStart(start s)
{
    exec = s;
}

void OSDTask::CreateCode(size_t code_size)
{
    //	CLogger::Get()->Write("OSDTask", LogDebug, "Create code: %d", code_size);
    this->code_size = code_size;
    code = new uint8_t[code_size];
    //	CLogger::Get()->Write("OSDTask", LogDebug, "Create code done");
}

uint8_t *OSDTask::GetCode()
{
    return code;
}

start OSDTask::GetExec()
{
    return exec;
}
