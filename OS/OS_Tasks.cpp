#include "OS.h"
#include "../Tasks/WindowManager/WindowManager.h"
#include "../Library/StringLib.h"
#include "../Tasks/TasksWindow/TasksWindow.h"
#include "../Tasks/Filer/Filer.h"
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/logger.h>
#include <circle/usertimer.h>

extern WindowManager* gui;
extern unsigned rate;
CTask* OSDTask::boot_task;
OSDTask* OSDTask::focus_task = NULL;
OSDTask* OSDTask::current_task;
extern CUserTimer* UserTimer;
std::list<OSDTask*> OSDTask::tasks_list;
size_t OSDTask::task_id = 0;
bool OSDTask::yield_due = false;
OSDTask* OSDTask::task_override = nullptr;

OSDTask* GetCurrentTask()
{
	if (OSDTask::GetOverride()!=nullptr)
		return OSDTask::GetOverride();
	return OSDTask::current_task;
}

OSDTask::OSDTask()
		:CTask()
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
		delete code;

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
	auto f = permanent_strings.extract(idx);
	strings.insert(std::move(f));
}

void OSDTask::ClearTemporaryStrings()
{
	strings.clear();
}

std::string& OSDTask::GetString(int64_t idx)
{
	auto f = strings.find(idx);
	if (f==strings.end()) {
		auto f = permanent_strings.find(idx);
		if (f==permanent_strings.end()) {
			CLogger::Get()->Write("OSDTask", LogDebug, "Temporary strings:");
			for (auto& s : strings) {
				CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
			}
			CLogger::Get()->Write("OSDTask", LogDebug, "Permanent strings:");
			for (auto& s : permanent_strings) {
				CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
			}
			CLogger::Get()->Write("OSDTask", LogPanic, "Invalid string: %d", idx);
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
	auto mScheduler = CScheduler::Get();
	auto task = mScheduler->GetTask(s);
	return (OSDTask*)task;
}

void OSDTask::Yield()
{
	if (terminate_requested)
		TerminateTask();
	if (maximise_requested)
		Maximise();
	if (minimise_requested)
		Minimise();
	auto mScheduler = CScheduler::Get();
	mScheduler->Yield();
}

void OSDTask::Sleep(int ms)
{
	if (terminate_requested)
		TerminateTask();
	if (maximise_requested)
		Maximise();
	if (minimise_requested)
		Minimise();
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

void OSDTask::RequestMaximise()
{
	maximise_requested = true;
}

void OSDTask::RequestMinimise()
{
	minimise_requested = true;
}

void OSDTask::Maximise()
{
	maximise_requested = false;
	CLogger::Get()->Write("Tasks Window", LogPanic, "Maximise not supported with this task type");
}

void OSDTask::Minimise()
{
	minimise_requested = false;
}

void OSDTask::ReceiveKey(Key k)
{
	keyboard_queue.push(k);
}

