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
//#define VERBOSE_COMPILE

void OSDTask::RunCode()
{
	auto t1 = GetClock();
	jit_clear_state();
	if (exec!=NULL)
		exec();
	double t2 = (double)(GetClock()-t1)/100.0;
	std::ostringstream out;
	out.precision(2);
	out << std::fixed << t2;

	// End message and wait for key
	/*		std::string finish_message = "Run time was "+out.str()+" seconds";
			Console_SetPosition((Console_Width()-finish_message.length())/2, Console_Height()-1);
			Console_SetFG(CONSOLE_BLACK);
			Console_SetBG(CONSOLE_YELLOW);
			Console_WriteString(finish_message.c_str());
			if (false)
				input->Get();*/
}

std::string OSDTask::LoadSource(std::string directory, std::string filename)
{
	fs.SetCurrentDirectory(directory);
	filename = fs.GetCurrentDirectory()+filename;

	FIL fil;
	if (f_open(&fil, (filename).c_str(), FA_READ | FA_OPEN_EXISTING)!=FR_OK) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error opening source file '%s'", filename.c_str());
	}
	size_t sz = f_size(&fil);
	char* buffer = (char*)malloc(sz+1);
	if (!buffer) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error allocating memory for source file '%s'", filename.c_str());
	}
	uint32_t l;
	if (f_read(&fil, buffer, sz, &l)!=FR_OK) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error loading source file '%s'", filename.c_str());
	}
	f_close(&fil);
	buffer[sz] = 0;
	std::string s(buffer);
	free(buffer);
	return s;
}

bool OSDTask::CompileSource(std::string filename, std::string code)
{
	const bool debug_output = true;
	const bool optimise = true;
	Tokeniser token(filename, code);
	Parser parser;
#ifdef VERBOSE_COMPILE
	double total_time_span = 0;
#endif

	// Tokens
#ifdef VERBOSE_COMPILE
	auto t1 = CTimer::GetClockTicks();
	token.Parse();
	auto t2 = CTimer::GetClockTicks();
	double time_span = (t2-t1)/1000.0;
	total_time_span += time_span;
	CLogger::Get()->Write("OSDTask", LogNotice, "Tokeniser: %f millis", time_span);
	if (debug_output) {
/*		std::list<std::string> tokens;
		token.PrintTokens(token.Tokens(), 0, &tokens);
		std::ofstream tokens_out("Tokeniser.txt");
		for (auto& s: tokens) {
			tokens_out << s << std::endl;
		}
		tokens_out.close();*/
	}
#else
	token.Parse();
#endif
#ifdef VERBOSE_COMPILE
	t1 = CTimer::GetClockTicks();
#endif
	parser.Parse(optimise, token.Tokens(), token.GetFilenames());
#ifdef VERBOSE_COMPILE
	t2 = CTimer::GetClockTicks();
	time_span = (t2-t1)/1000.0;
	total_time_span += time_span;
	CLogger::Get()->Write("OSDTask", LogNotice, "Parser: %f millis", time_span);
	if (debug_output) {
/*		std::list<std::string> tokens;
		token.PrintTokensPtr(parser.Tokens(), 0, &tokens);
		std::ofstream tokens_out("../../tokens.txt");
		for (auto& s: tokens) {
			tokens_out << s << std::endl;
		}
		tokens_out.close();*/
	}
#endif

// Compile
#ifdef VERBOSE_COMPILE
	t1 = CTimer::GetClockTicks();
#endif
	IRCompiler ir_compiler(optimise, token.GetFilenames());
	ir_compiler.Compile(parser.Tokens());
#ifdef VERBOSE_COMPILE
	t2 = CTimer::GetClockTicks();
	time_span = (t2-t1)/1000.0;
	total_time_span += time_span;
	CLogger::Get()->Write("OSDTask", LogNotice, "IR Compiler: %f millis", time_span);
#endif
#ifdef VERBOSE_COMPILE
	if (debug_output) {
/*		std::list<std::string> ir;
		ir_compiler.IRPrinter(&ir);
		std::ofstream ir_out("../../ir.txt");
		for (auto& s: ir) {
			ir_out << s << std::endl;
		}
		ir_out.close();*/
	}
#endif

// Native
	_jit = jit_new_state();
#ifdef VERBOSE_COMPILE
	t1 = CTimer::GetClockTicks();
#endif
	NativeCompiler native_compiler(optimise, _jit, this);
	native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
#ifdef VERBOSE_COMPILE
	t2 = CTimer::GetClockTicks();
	time_span = (t2-t1)/1000.0;
	total_time_span += time_span;
	CLogger::Get()->Write("OSDTask", LogNotice, "Native Compiler: %f millis", time_span);
#endif
#ifdef VERBOSE_COMPILE
	if (debug_output) {
/*		std::list<std::string> disassm;
		native_compiler.Disassemble(&disassm);
		std::ofstream native_out("../../native.txt");
		for (auto& s: disassm) {
			native_out << s << std::endl;
		}
		native_out.close();*/
	}
#endif
#ifdef VERBOSE_COMPILE
	CLogger::Get()->Write("OSDTask", LogNotice, "Total: %f millis", total_time_span);
#endif
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

uint8_t* OSDTask::GetCode()
{
	return code;
}

start OSDTask::GetExec()
{
	return exec;
}

