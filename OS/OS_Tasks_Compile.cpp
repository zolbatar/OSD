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
    if (debug)
        Breakdown::Init();

    // Tokens
    auto t1 = CTimer::GetClockTicks();
    token.Parse();
    auto t2 = CTimer::GetClockTicks();
    double time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    if (debug)
        Breakdown::ProcessTokeniser(&token);

    // Parser
    t1 = CTimer::GetClockTicks();
    parser.Parse(optimise, token.Tokens(), token.GetFilenames());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    //    CLogger::Get()->Write("OSDTask", LogNotice, "Parser: %f millis", time_span);
    if (debug)
        Breakdown::ProcessParser(&token, &parser);

    // Compile
    t1 = CTimer::GetClockTicks();
    IRCompiler ir_compiler(optimise, token.GetFilenames());
    ir_compiler.Compile(parser.Tokens());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    //    CLogger::Get()->Write("OSDTask", LogNotice, "IR Compiler: %f millis", time_span);
    if (debug)
        Breakdown::ProcessIR(&ir_compiler);

    // Native
    _jit = jit_new_state();
    t1 = CTimer::GetClockTicks();
    NativeCompiler native_compiler(optimise, _jit, this);
    native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions(), debug);
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    //    CLogger::Get()->Write("OSDTask", LogNotice, "Native Compiler: %f millis", time_span);
    if (debug)
        Breakdown::ProcessNative(&native_compiler);

    // Output new style debug
    if (debug)
        Breakdown::Output(&this->fs, volume, directory, filename);

    return true;
}

void OSDTask::SetStart(start s)
{
    //    CLogger::Get()->Write("OSDTask", LogNotice, "Set start: %p", s);
    exec = s;
}

void OSDTask::CreateCode(size_t code_size)
{
    if (code != NULL)
    {
        delete code;
    }
    this->code_size = code_size;
    code = new uint8_t[code_size];
}

uint8_t *OSDTask::GetCode()
{
    return code;
}

start OSDTask::GetExec()
{
    return exec;
}
