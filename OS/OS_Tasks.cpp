#include <iostream>
#include "OS.h"
#include "../Tasks/WindowManager/WindowManager.h"
#include "../Library/StringLib.h"
#include <sstream>
#include "../Parser/Parser.h"
#include "../Tokeniser/Tokeniser.h"
#include "../IRCompiler/IRCompiler.h"
#include "../NativeCompiler/NativeCompiler.h"
#include "../Exception/DARICException.h"
#include "../Chrono/Chrono.h"
#include "../Input/Input.h"
#include "../Tasks/TasksWindow/TasksWindow.h"
#include "../Tasks/Filer/Filer.h"

#ifndef CLION
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/logger.h>
#include <circle/usertimer.h>
#endif

extern WindowManager* gui;
extern Input* input;

#ifdef CLION
std::map<std::string, OSDTask*> OSDTask::tasks;
std::map<std::thread::id, OSDTask*> OSDTask::task_threads;
#else
extern unsigned rate;
CTask *OSDTask::boot_task;
OSDTask *OSDTask::current_task;
extern CUserTimer* UserTimer;
#endif
std::list<OSDTask*> OSDTask::tasks_list;
size_t OSDTask::task_id = 0;
bool OSDTask::yield_due = false;
OSDTask* OSDTask::task_override = nullptr;

OSDTask* GetCurrentTask()
{
	if (OSDTask::GetOverride()!=nullptr)
		return OSDTask::GetOverride();
#ifndef CLION
	return OSDTask::current_task;
//	auto mScheduler = CScheduler::Get();
//		return (OSDTask *)mScheduler->GetCurrentTask();
#else
	auto id = std::this_thread::get_id();
	auto f = OSDTask::task_threads.find(id);
	assert(f!=OSDTask::task_threads.end());
	return f->second;
#endif
}

OSDTask::OSDTask()
#ifndef CLION
: CTask()
#endif
{
	// To be safe, zero these out
	for (auto it = allocations.begin(); it!=allocations.end(); ++it) {
		it->m = 0;
	}
	fs.Init();
}

OSDTask::~OSDTask()
{
	if (_jit!=NULL)
		jit_destroy_state();
	if (code!=NULL)
		DELETE code;

	// Delete any allocations, FIXME
/*	for (auto& alloc : allocations) {
		if (alloc.m!=0)
			free(alloc.m);
	}*/
}

void OSDTask::SetNameAndAddToList()
{
	SetName(id.c_str());
	tasks_list.push_back(this);
}

void OSDTask::TerminateTask()
{
	terminate_requested = false;
	// Close window
	DirectMessage mess;
	mess.type = Messages::WM_CloseWindow;
	mess.source = this;
	CallGUIDirectEx(&mess);

	// Remove
	tasks_list.remove(this);
	Terminate();
}

void OSDTask::TaskTerminationHandler(CTask* ctask)
{
	OSDTask::current_task = (OSDTask*)ctask;
	switch (current_task->type) {
		case TaskType::DARIC: {
			auto dw = (DARICWindow*)ctask;
			delete dw;
			break;
		}
		case TaskType::Filer: {
			auto dw = (Filer*)ctask;
			delete dw;
			break;
		}
		case TaskType::TaskManager: {
			auto dw = (TasksWindow*)ctask;
			delete dw;
			break;
		}
		case TaskType::Other:
			delete current_task;
			break;
	}
}

void OSDTask::TaskSwitchHandler(CTask* ctask)
{
	OSDTask::current_task = (OSDTask*)ctask;
	switch (current_task->priority) {
		case TaskPriority::NoPreempt:
			UserTimer->Start(rate*4096);
			break;
		case TaskPriority::High:
			UserTimer->Start(rate*5);
			break;
		default:
			UserTimer->Start(rate);
			break;
	}
//	CLogger::Get()->Write("OS/D", LogNotice, "Task focus: %s", ctask->GetName());
}

void OSDTask::ReceiveDirectEx(DirectMessage* m)
{
	assert(0);
}

void OSDTask::CallGUIDirectEx(DirectMessage* m)
{
	if (terminate_requested)
		TerminateTask();
	if (yield_due) {
		yield_due = false;
		Yield();
	}
	GetTask("@")->ReceiveDirectEx(m);
}

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
#ifdef CLION
//	printf("String size: %zu\n", strings.size());
#endif
	return i;
}

int64_t OSDTask::AddStringPermanent(std::string s)
{
	// Do we have an existing one the same?
	for (auto& str: permanent_strings) {
		if (str.second==s) {
			return str.first;
		}
	}
	auto i = string_index++;
	constant_strings.insert(i);
	permanent_strings.insert(std::make_pair(i, std::move(s)));
	return i;
}

void OSDTask::MakeStringPermanent(int64_t idx)
{
	auto f = strings.extract(idx);
	permanent_strings.insert(std::move(f));
	GetCurrentTask()->ClearTemporaryStrings();
}

void OSDTask::SetConstantString(int64_t idx)
{
	constant_strings.insert(idx);
}

void OSDTask::FreeStringPermanent(int64_t idx)
{
	if (constant_strings.count(idx)>0)
		return;
#ifdef CLION
	if (permanent_strings.count(idx)==0) {
		return;
//		printf("%ld %d\n", idx, strings.count(idx));
//		assert(0);
	}
#endif
	auto f = permanent_strings.extract(idx);
	strings.insert(std::move(f));
}

void OSDTask::ClearTemporaryStrings()
{
#ifdef CLION
//	printf("Deleting %zu strings\n", strings.size());
#endif
	strings.clear();
}

std::string& OSDTask::GetString(int64_t idx)
{
	auto f = strings.find(idx);
	if (f==strings.end()) {
		auto f = permanent_strings.find(idx);
		if (f==permanent_strings.end()) {
#ifdef CLION
			printf("Invalid string");
			exit(1);
#else
			CLogger::Get()->Write("OSDTask", LogDebug, "Temporary strings:");
			for (auto &s : strings) {
				CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
			}
			CLogger::Get()->Write("OSDTask", LogDebug, "Permanent strings:");
			for (auto &s : permanent_strings) {
				CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
			}
			CLogger::Get()->Write("OSDTask", LogPanic, "Invalid string: %d", idx);
#endif
		}
		return f->second;
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
#ifdef CLION
	const bool optimise = false;
#else
	const bool optimise = true;
#endif
	Tokeniser token(filename, code);
	Parser parser;
#ifdef CLION
	double total_time_span = 0;
#endif

	// Tokens
#ifdef CLION
	auto t1 = std::chrono::system_clock::now();
	token.Parse();
	auto t2 = std::chrono::system_clock::now();
	double time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	time_span /= 1000.0;
	total_time_span += time_span;
	printf("Tokeniser: %f millis\n", time_span);
	if (debug_output) {
		std::list<std::string> tokens;
		token.PrintTokens(token.Tokens(), 0, &tokens);
		std::ofstream tokens_out("../../tokeniser.txt");
		for (auto& s: tokens) {
			tokens_out << s << std::endl;
		}
		tokens_out.close();
	}
#else
	token.Parse();
#endif
#ifdef CLION
	t1 = std::chrono::system_clock::now();
#endif
	parser.Parse(optimise, token.Tokens(), token.GetFilenames());
#ifdef CLION
	t2 = std::chrono::system_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	time_span /= 1000.0;
	total_time_span += time_span;
	printf("Parser: %f millis\n", time_span);
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
	IRCompiler ir_compiler(optimise, token.GetFilenames());
	ir_compiler.Compile(parser.Tokens());
#ifdef CLION
	t2 = std::chrono::system_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	time_span /= 1000.0;
	total_time_span += time_span;
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
	NativeCompiler native_compiler(optimise, _jit, this);
	native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
#ifdef CLION
	t2 = std::chrono::system_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
	time_span /= 1000.0;
	total_time_span += time_span;
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
#ifdef CLION
	printf("Total: %f millis\n", total_time_span);
#endif
	return true;
}

void OSDTask::Yield()
{
	if (terminate_requested)
		TerminateTask();
	auto mScheduler = CScheduler::Get();
	mScheduler->Yield();
}

void OSDTask::Sleep(int ms)
{
	if (terminate_requested)
		TerminateTask();
	auto mScheduler = CScheduler::Get();
	mScheduler->MsSleep(ms);
}

size_t OSDTask::CalculateMemoryUsed()
{
	size_t r = 0;

	// Allocation
	for (auto& m: allocations) {
		if (m.m!=0)
			r += m.sz;
	}
	return r;
}

void OSDTask::UpdateGUI()
{
	assert(0);
}

void OSDTask::SetStart(start s)
{
	exec = s;
}

void OSDTask::CreateCode(size_t code_size)
{
//	CLogger::Get()->Write("OSDTask", LogDebug, "Create code: %d", code_size);
	this->code_size = code_size;
	code = NEW uint8_t[code_size];
//	CLogger::Get()->Write("OSDTask", LogDebug, "Create code done");
}

uint8_t* OSDTask::GetCode() { return code; }

start OSDTask::GetExec()
{
	return exec;
}

void OSDTask::SetOverride(OSDTask* task)
{
	task_override = task;
}

void OSDTask::ClearOverride()
{
	task_override = nullptr;
}

OSDTask* OSDTask::GetOverride()
{
	return task_override;
}

void OSDTask::RequestTerminate()
{
	terminate_requested = true;
}


