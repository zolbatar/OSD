#include "OS.h"

#ifndef CLION
#include <circle/logger.h>
#else
#endif

extern size_t kernel_size;
extern size_t initial_mem_free;
extern size_t pre_boot_memory;

void OS_Init()
{
}

struct AllocRef {
	OSDTask* task;
	void* m;
	size_t sz;
};

void TaskAddAllocation(size_t size, void* m)
{
	if (kernel_size==0)return;
	auto task = GetCurrentTask();
#ifndef CLION
	if (task==OSDTask::boot_task) {
		pre_boot_memory += size;
		return;
	}
#endif
	GetCurrentTask()->AddAllocation(size, m);
}

void TaskFreeAllocation(void* m)
{
	if (kernel_size==0)return;

	// Try current task first
	auto task = GetCurrentTask();
	if (task->FreeAllocation(m))
		return;

	// Fall back and try all
	for (auto& task: OSDTask::tasks_list) {
		if (task->FreeAllocation(m))
			return;
	}
//    assert(0);
}

void CalculateMem(MemorySummary* m)
{
#ifndef CLION
	auto mem = CMemorySystem::Get();
	m->total_memory = (mem->GetMemSize()-kernel_size);
	m->free_memory = mem->GetHeapFreeSpace(HEAP_ANY);
	m->total_task = 0;
	for (auto& task: OSDTask::tasks_list) {
		m->total_task += task->CalculateMemoryUsed();
	}
	m->used = initial_mem_free-mem->GetHeapFreeSpace(HEAP_ANY);
	m->lost = m->used-m->total_task;
#else
	m->total_memory = 1;
	m->free_memory = 1;
	m->total_task = 1;
	m->used = 1;
	m->lost = 1;
#endif
}

void DumpMemory()
{
	MemorySummary m;
	CalculateMem(&m);
#ifndef CLION
	CLogger::Get()->Write("Memory", LogDebug, "Total memory: %d", m.total_memory);
	CLogger::Get()->Write("Memory", LogDebug, "Free memory: %d", m.free_memory);
	CLogger::Get()->Write("Memory", LogDebug, "Kernel size: %d", kernel_size);
	CLogger::Get()->Write("Memory", LogDebug, "Total task memory: %d", m.total_task);
	CLogger::Get()->Write("Memory", LogDebug, "Used memory: %d", m.used);
	CLogger::Get()->Write("Memory", LogDebug, "Lost memory: %d", m.lost);
	CLogger::Get()->Write("Memory", LogDebug, "Pre-boot memory: %d", pre_boot_memory);
	CLogger::Get()->Write("Memory", LogDebug, "---");
	for (auto &task: OSDTask::tasks_list) {
	}
#endif
}

