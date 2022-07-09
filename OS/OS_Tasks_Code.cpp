#include "OS_Tasks_Code.h"
#include <Breakdown/Breakdown.h>
#include <Chrono/Chrono.cpp>

OSDTaskCode::OSDTaskCode()
{
}

OSDTaskCode::~OSDTaskCode()
{
    if (_jit != NULL)
        jit_destroy_state();
    if (code != NULL)
        delete code;
}

void OSDTaskCode::RunCode(bool wait)
{
    this->wait = wait;
    t1 = GetClock();
    jit_clear_state();
    if (exec != NULL)
        exec();

    // We never return here
}

std::string OSDTaskCode::LoadSource(FileSystem *fs, std::string volume, std::string directory, std::string filename)
{
    fs->SetVolume(volume);
    fs->SetCurrentDirectory(directory);
    filename = fs->GetCurrentDirectory() + filename;

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

std::string OSDTaskCode::CompileSource(FileSystem *fs, std::string volume, std::string directory, std::string filename,
                                       std::string code, bool debug)
{
    std::string messages;
    Breakdown::Init();

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
    messages += "Tokeniser: " + std::to_string((int)time_span) + "ms\n";
    if (debug)
        Breakdown::ProcessTokeniser(&token);

    // Parser
    t1 = CTimer::GetClockTicks();
    parser.Parse(optimise, token.Tokens(), token.GetFilenames());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    messages += "Parser: " + std::to_string((int)time_span) + "ms\n";
    if (debug)
        Breakdown::ProcessParser(&token, &parser);

    // Compile
    t1 = CTimer::GetClockTicks();
    IRCompiler ir_compiler(optimise, token.GetFilenames());
    ir_compiler.Compile(parser.Tokens());
    t2 = CTimer::GetClockTicks();
    time_span = (t2 - t1) / 1000.0;
    total_time_span += time_span;
    messages += "IR Compiler: " + std::to_string((int)time_span) + "ms\n";
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
    messages += "Native Compiler: " + std::to_string((int)time_span) + "ms\n";
    if (debug)
        Breakdown::ProcessNative(&native_compiler);

    // Output new style debug
    if (debug)
    {
        fs->SetVolume(volume);
        fs->SetCurrentDirectory(directory);
        filename = fs->GetCurrentDirectory() + filename;
        Breakdown::Output(fs, volume, directory, filename);
    }

    messages += "Total Time: " + std::to_string((int)total_time_span) + "ms\n";
    return messages;
}

void OSDTaskCode::SetStart(start s)
{
    //    CLogger::Get()->Write("OSDTask", LogNotice, "Set start: %p", s);
    exec = s;
}

void OSDTaskCode::CreateCode(size_t code_size)
{
    if (code != NULL)
    {
        delete code;
    }
    this->code_size = code_size;
    code = new uint8_t[code_size];
}

uint8_t *OSDTaskCode::GetCode()
{
    return code;
}

start OSDTaskCode::GetExec()
{
    return exec;
}
