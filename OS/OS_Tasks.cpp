#include "OS.h"
#include <iostream>
#include "../Tasks/GUI.h"

#ifdef CLION
#include <fstream>
#endif

#include <sstream>
#include "../Parser/Parser.h"
#include "../Tokeniser/Tokeniser.h"
#include "../IRCompiler/IRCompiler.h"
#include "../NativeCompiler/NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../Chrono/Chrono.h"
#include "../Input/Input.h"

#ifndef CLION
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#endif

extern Input* input;
std::string string_error = "NOT A VALID STRING";

#ifdef CLION
std::map<std::string, OSDTask*> OSDTask::tasks;
#endif

OSDTask* GetCurrentTask()
{
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	return (OSDTask *)mScheduler->GetCurrentTask();
#else
	return OSDTask::tasks.begin()->second;
#endif
}

void OSDTask::TerminateTask()
{
	// Close window
	SendGUIMessage(Messages::WM_CloseWindow, NULL);
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w!=NULL);

#ifndef CLION
	auto mScheduler = CScheduler::Get();
	auto task = (OSDTask *)mScheduler->GetCurrentTask();
	task->Terminate();
#else
#endif
}

#ifdef CLION

void OSDTask::Run()
{
	assert(0);
}

#endif

void OSDTask::RunCode()
{
	if (exec!=NULL) {
		exec();
	}
}

void OSDTask::AddDataElement(DataElement de)
{
	data_elements.push_back(std::move(de));
	data_element_index++;
}

DataElement* OSDTask::GetDataElement()
{
	if (data_element_index>=data_elements.size())
		data_element_index = 0;
	auto de = &data_elements[data_element_index];
	data_element_index++;
	return de;
}

void OSDTask::AddDataLabel(std::string s)
{
	data_labels.insert(std::make_pair(s, data_element_index));
}

void OSDTask::RestoreDataLabel(std::string s)
{
	auto f = data_labels.find(s);
	if (f==data_labels.end())
		return;
	data_element_index = f->second;
}

int64_t OSDTask::AddString(std::string s)
{
	auto i = string_index++;
	strings.insert(std::make_pair(i, std::move(s)));
	return i;
}

std::string& OSDTask::GetString(int64_t idx)
{
	auto f = strings.find(idx);
	if (f==strings.end()) {
		return string_error;
	}
	return f->second;
}

void OSDTask::AddAllocation(size_t size, void* m)
{
	allocations.insert(std::make_pair(m, size));
}

void OSDTask::FreeAllocation(void* m)
{
	free(m);
	auto f = allocations.find(m);
	if (f==allocations.end())
		return;
	allocations.erase(f);
}

void OSDTask::SendMessage(Messages m, void* data, OSDTask* source)
{
	while (message_queue.size_approx()>=max_message_queue) {
		Yield();
	}
	message_queue.enqueue(Message(m, (void*)source, data));
}

void OSDTask::SendGUIMessage(Messages m, void* data)
{
	auto gui = GetTask("GUI");
	gui->SendMessage(m, data, this);
}

OSDTask* OSDTask::GetTask(const char* s)
{
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	auto task = mScheduler->GetTask(s);
	return (OSDTask*)task;
#else
	std::string ss(s);
	std::map<std::string, OSDTask*>::iterator f;
	do {
		f = tasks.find(s);
	}
	while (f==tasks.end());
	return f->second;
#endif
}

void OSDTask::CompileSource(std::string code)
{
	Tokeniser token;
	Parser parser;
	try {
		// Tokens
		token.Parse("[Interactive]", code);
		parser.Parse(true, token.Tokens());
#ifdef CLION
		std::list<std::string> tokens;
		token.PrintTokensPtr(parser.Tokens(), 0, &tokens);
		std::ofstream tokens_out("../../tokens.txt");
		for (auto& s: tokens) {
			tokens_out << s << std::endl;
		}
		tokens_out.close();
#endif

		// Compile
		IRCompiler ir_compiler(true);
		ir_compiler.Compile(parser.Tokens());
#ifdef CLION
		std::list<std::string> ir;
		ir_compiler.IRPrinter(&ir);
		std::ofstream ir_out("../../ir.txt");
		for (auto& s: ir) {
			ir_out << s << std::endl;
		}
		ir_out.close();
#endif

		// Native
		NativeCompiler native_compiler(true);
		native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
#ifdef CLION
		std::list<std::string> disassm;
		native_compiler.Disassemble(&disassm);
		std::ofstream native_out("../../native.txt");
		for (auto& s: disassm) {
			native_out << s << std::endl;
		}
		native_out.close();
#endif

		auto t1 = GetClock();
		RunCode();
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
	catch (DARICException& ex) {
		switch (ex.type) {
			case ExceptionType::COMPILER:
				printf("[Compiler] ");
				break;
			case ExceptionType::TOKENISER:
				printf("[Tokeniser] ");
				break;
			case ExceptionType::PARSER:
				printf("[Parser] ");
				break;
			case ExceptionType::RUNTIME:
				printf("[Runtime] ");
				break;
		}
		printf("%s at line %d, column %d", ex.error.c_str(), ex.line_number, ex.char_position);
	}
}