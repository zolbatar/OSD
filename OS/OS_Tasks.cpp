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
#include <circle/logger.h>
#endif

extern Input* input;
std::string string_error = "NOT A VALID STRING";

#ifdef CLION
std::map<std::string, OSDTask*> OSDTask::tasks;
#endif
std::list<OSDTask*> OSDTask::tasks_list;
size_t OSDTask::task_id = 0;

OSDTask* GetCurrentTask()
{
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	return (OSDTask *)mScheduler->GetCurrentTask();
#else
	return OSDTask::tasks.begin()->second;
#endif
}

OSDTask::~OSDTask()
{
	if (code!=NULL)
		DELETE code;
}

void OSDTask::TerminateTask()
{
	// Close window
	auto mess = SendGUIMessage();
	mess->type = Messages::WM_CloseWindow;
	mess->source = this;
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w!=NULL);

	// Remove
#ifdef CLION
	tasks.remove(this);
#endif
	tasks_list.remove(this);

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
	// Scan for hole
	for (auto it = allocations.begin(); it!=allocations.end(); ++it) {
		if (it->m==0) {
			it->m = m;
			it->sz = size;
			return;
		}
	}

	// Need more space
	allocations.emplace_back(TaskAllocRef{ m, size });
}

void OSDTask::FreeAllocation(void* m)
{
	// Go backwards (as more likely to free the last allocation)
	for (auto it = allocations.rbegin(); it!=allocations.rend(); ++it) {
		if (it->m==m) {
			it->m = 0;
			return;
		}
	}
}

Message* OSDTask::SendMessage()
{
	while (message_queue_position==MAX_MESSAGE_QUEUE) {
		Yield();
		CLogger::Get()->Write("GUI", LogDebug, "Full thread");
	}
	Message* m = &message_queue[message_queue_position++];
	return m;
}

Message* OSDTask::SendGUIMessage()
{
	auto gui = GetTask("GUI");
	return gui->SendMessage();
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
		f = tasks.find(ss);
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
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

void OSDTask::Yield()
{
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	mScheduler->Yield();
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
#endif
}

void OSDTask::Sleep(int ms)
{
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	mScheduler->MsSleep(ms);
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
}

size_t OSDTask::CalculateMemoryUsed()
{
	size_t r = 0;

	// Allocation
	for (auto& m : allocations) {
		if (m.m!=0)
			r += m.sz;
	}

	// Strings
	for (auto& m : strings) {
		r += m.second.size();
	}

	// Code size
	r += code_size;

	// Stack
	r += STACK_SIZE;

	// This (not sure how accurate this is)
	r += sizeof(this);

	return r;
}
