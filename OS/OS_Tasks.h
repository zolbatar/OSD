#pragma once
#ifndef CLION
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/new.h>
#include <circle/alloc.h>
#define NEW new(HEAP_ANY)
#else

#define NEW new
#endif
#define DELETE delete

#include <vector>
#include <list>
#include "OS_Messages.h"
#include "../Concurrent/concurrentqueue.h"

const size_t MIN_MESSAGE_QUEUE = 64;
const size_t MAX_MESSAGE_QUEUE = 4096;

extern std::string string_error;
typedef void (* start)(void);

struct TaskAllocRef {
	void* m = 0;
	size_t sz;
};

struct DataElement {
	ValueType type;
	int64_t iv = 0;
	double rv = 0.0;
	int64_t sv = 0;
};

struct Message {
	Messages type;
	void* source; // This is an OSDTask
	uint8_t data[MESSAGE_BLOCK];
};

#ifndef CLION
class OSDTask : public CTask {
#else

class OSDTask {
#endif
public:
	OSDTask()
#ifndef CLION
	: CTask()
#endif
	{
	}

	~OSDTask();

	size_t CalculateMemoryUsed();

	void SetNameAndAddToList()
	{
#ifndef CLION
		SetName(name.c_str());
#else
		SetName(name);
		tasks.insert(std::make_pair(name, this));
#endif
		tasks_list.push_back(this);
	}

#ifdef CLION

	virtual void Run();

	void SetName(std::string name)
	{
		this->name = name;
	}

	void Start()
	{
		Run();
	}

	void WaitForTermination()
	{
	}

#endif

	void CompileSource(std::string code);
	void RunCode();
	void AddDataElement(DataElement de);
	DataElement* GetDataElement();
	void AddDataLabel(std::string s);
	void RestoreDataLabel(std::string s);
	int64_t AddString(std::string s);
	std::string& GetString(int64_t idx);

	// Malloc
	void AddAllocation(size_t size, void* m);
	bool FreeAllocation(void* m);

	void SetStart(start s)
	{
		exec = s;
	}

	void CreateCode(size_t code_size)
	{
		this->code_size = code_size;
		code = NEW uint8_t[code_size];
	}

	uint8_t* GetCode() { return code; }

	start GetExec()
	{
		return exec;
	}

	Message* SendMessage();
	Message* SendGUIMessage();
	void Yield();
	void Sleep(int ms);

	void SetWindow(std::string id, void* w)
	{
		this->id = id;
		this->w = w;
	}

	std::string GetWindowID() { return id; }
	std::string GetWindowName() { return name; }

	void* GetWindow() { return w; }

	bool IsExclusive() { return exclusive; }

	void TerminateTask();

#ifdef CLION
	static std::map<std::string, OSDTask*> tasks;
#endif
	static std::list<OSDTask*> tasks_list;
#ifndef CLION
	static CTask *boot_task;
#endif
protected:
	OSDTask* GetTask(const char* s);
	Message* message_queue = NULL;
	size_t message_queue_position = 0;
	size_t message_queue_size = 0;
	start exec;
	bool exclusive = false;
	int d_x;
	int d_y;
	int d_w;
	int d_h;
	static size_t task_id;
	std::string id;
	std::string name;
private:
	void* w = NULL;
	int64_t idx;
	int64_t string_index = 0;
	size_t data_element_index = 0;
	std::vector<TaskAllocRef> allocations;
	std::map<int64_t, std::string> strings;
	std::map<std::string, size_t> data_labels;
	std::vector<DataElement> data_elements;
	size_t code_size;
	uint8_t* code = NULL;
};