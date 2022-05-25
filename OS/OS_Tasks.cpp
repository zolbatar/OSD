#include "OS.h"
#include <iostream>
#include "../Tasks/WindowManager/WindowManager.h"

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

OSDTask* OSDTask::task_override = NULL;
#ifdef CLION
std::map<std::string, OSDTask*> OSDTask::tasks;
std::map<std::thread::id, OSDTask*> OSDTask::task_threads;
std::mutex OSDTask::vlgl_mutex;
#else
CTask *OSDTask::boot_task;
#endif
std::list<OSDTask*> OSDTask::tasks_list;
size_t OSDTask::task_id = 0;

OSDTask* GetCurrentTask()
{
	// Do we have an override?
	auto tover = OSDTask::GetTaskOverride();
	if (tover!=NULL)
		return tover;
#ifndef CLION
	auto mScheduler = CScheduler::Get();
	return (OSDTask *)mScheduler->GetCurrentTask();
#else
	auto id = std::this_thread::get_id();
	auto f = OSDTask::task_threads.find(id);
	assert(f!=OSDTask::task_threads.end());
	return f->second;
#endif
}

OSDTask::~OSDTask()
{
	if (_jit!=NULL)
		jit_destroy_state();
	if (code!=NULL)
		DELETE code;
	if (message_queue!=NULL)
		DELETE message_queue;
}

void OSDTask::SetNameAndAddToList()
{
#ifndef CLION
	SetName(id.c_str());
#else
	SetName(id);
	tasks.insert(std::make_pair(id, this));
	auto id = std::this_thread::get_id();
	task_threads.insert(std::make_pair(id, this));
#endif
	tasks_list.push_back(this);
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
	tasks_list.remove(this);
#ifdef CLION
	OSDTask::tasks.erase(this->name);
#else
	Terminate();
#endif
}

#ifndef CLION
void OSDTask::TaskTerminationHandler(CTask* ctask)
{
	auto tt = (TaskType*)ctask->GetUserData(TASK_USER_DATA_USER);
	switch (*tt) {
		case TaskType::DARIC: {
			auto dw = (DARICWindow*)ctask;
			delete dw;
			break;
		}
		default:
			CLogger::Get()->Write("OS_Tasks", LogDebug, "Unknown terminator handler type: %p/%d", tt, *tt);
			break;
	}
}
#endif

#ifdef CLION

void OSDTask::Run()
{
	assert(0);
}

#endif

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

void OSDTask::FreeString(int64_t index)
{
	strings.erase(index);
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
}

bool OSDTask::FreeAllocation(void* m)
{
	// Go backwards (as more likely to free the last allocation)
	for (auto it = allocations.rbegin(); it!=allocations.rend(); ++it) {
		if (it->m==m) {
			it->m = 0;
			return true;
		}
	}
//	assert(0);
	return false;
}

size_t OSDTask::GetAllocCount()
{
	size_t i = ALLOCATION_SIZE-1;
	for (auto it = allocations.rbegin(); it!=allocations.rend(); ++it) {
		if (it->m!=0) {
			return i;
		}
		i--;
	}
	return 0;
}

Message* OSDTask::SendMessage()
{
	if (message_queue==NULL) {
		message_queue_size = MIN_MESSAGE_QUEUE;
		message_queue = NEW Message[message_queue_size];
	}
	while (message_queue_position==message_queue_size) {
		Yield();
#ifndef CLION
		CLogger::Get()->Write("GUI", LogDebug, "Full thread");
#else
		printf("Full thread");
#endif
	}
	Message* m = &message_queue[message_queue_position++];
	return m;
}

Message* OSDTask::SendGUIMessage()
{
	auto gui = GetTask("@");
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
	const bool debug_output = false;
	Tokeniser token;
	Parser parser;
	try {
		// Tokens
#ifdef CLION
		auto t1 = std::chrono::system_clock::now();
#endif
		token.Parse("[Interactive]", code);
		parser.Parse(true, token.Tokens());
#ifdef CLION
		auto t2 = std::chrono::system_clock::now();
		double time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
		time_span /= 1000.0;
		printf("Parser: %f millis\n", time_span);
#endif
#ifdef CLION
		if (debug_output) {
			std::list<std::string> tokens;
			token.PrintTokensPtr(parser.Tokens(), 0, &tokens);
			std::ofstream tokens_out("../../tokens.txt");
			for (auto& s: tokens) {
				tokens_out << s << std::endl;
			}
			tokens_out.close();
		}
#endif

		// Compile
#ifdef CLION
		t1 = std::chrono::system_clock::now();
#endif
		IRCompiler ir_compiler(true);
		ir_compiler.Compile(parser.Tokens());
#ifdef CLION
		t2 = std::chrono::system_clock::now();
		time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
		time_span /= 1000.0;
		printf("IR Compiler: %f millis\n", time_span);
#endif
#ifdef CLION
		if (debug_output) {
			std::list<std::string> ir;
			ir_compiler.IRPrinter(&ir);
			std::ofstream ir_out("../../ir.txt");
			for (auto& s: ir) {
				ir_out << s << std::endl;
			}
			ir_out.close();
		}
#endif

		// Native
		_jit = jit_new_state();
#ifdef CLION
		t1 = std::chrono::system_clock::now();
#endif
		NativeCompiler native_compiler(true, _jit, this);
		native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
#ifdef CLION
		t2 = std::chrono::system_clock::now();
		time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
		time_span /= 1000.0;
		printf("Native Compiler: %f millis\n", time_span);
#endif
#ifdef CLION
		if (debug_output) {
			std::list<std::string> disassm;
			native_compiler.Disassemble(&disassm);
			std::ofstream native_out("../../native.txt");
			for (auto& s: disassm) {
				native_out << s << std::endl;
			}
			native_out.close();
		}
#endif
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
#ifdef CLION
		printf("%s at line %d, column %d", ex.error.c_str(), ex.line_number, ex.char_position);
#else
		CLogger::Get()->Write("CompileSource", LogPanic, "%s at line %d, column %d", ex.error.c_str(), ex.line_number, ex.char_position);
#endif
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
	for (auto& m: allocations) {
		if (m.m!=0)
			r += m.sz;
	}

	// Strings
/*	for (auto& m: strings) {
		r += m.second.size();
	}*/

	return r;
}
