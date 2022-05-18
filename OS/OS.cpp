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

const int MAX_ALLOC_REF = 8192;
static size_t ref_count_bank1 = 0;
static size_t ref_count_bank2 = 0;
static AllocRef refs_bank1[MAX_ALLOC_REF];
static AllocRef refs_bank2[MAX_ALLOC_REF];
static bool bank = false;

void ProcessAllocList()
{
	// Flip to other bank
	if (!bank) {
		bank = !bank;
		for (size_t i = 0; i<ref_count_bank1; i++) {
			AllocRef* ar = &refs_bank1[i];
			ar->task->AddAllocation(ar->sz, ar->m);
		}
		ref_count_bank1 = 0;
	}
	else {
		bank = !bank;
		for (size_t i = 0; i<ref_count_bank2; i++) {
			AllocRef* ar = &refs_bank2[i];
			ar->task->AddAllocation(ar->sz, ar->m);
		}
		ref_count_bank2 = 0;
	}
}

void TaskAddAllocation(size_t size, void* m)
{
	if (kernel_size==0)return;
	auto task = GetCurrentTask();
	if (task==OSDTask::boot_task) {
		pre_boot_memory += size;
		return;
	}
	if (!bank) {
		auto ar = &refs_bank1[ref_count_bank1++];
		ar->m = m;
		ar->sz = size;
		ar->task = task;
		if (ref_count_bank1==MAX_ALLOC_REF) {
			ProcessAllocList();
		}
	}
	else {
		auto ar = &refs_bank2[ref_count_bank2++];
		ar->m = m;
		ar->sz = size;
		ar->task = task;
		if (ref_count_bank2==MAX_ALLOC_REF) {
			ProcessAllocList();
		}
	}
}

void TaskFreeAllocation(void* m)
{
	if (kernel_size==0)return;
	auto task = GetCurrentTask();
	task->FreeAllocation(m);
}

MemorySummary CalculateMem()
{
	ProcessAllocList();

	MemorySummary m;
	auto mem = CMemorySystem::Get();
	m.total_memory = (mem->GetMemSize()-kernel_size);
	m.free_memory = mem->GetHeapFreeSpace(HEAP_ANY);
	m.total_task = 0;
	for (auto& task: OSDTask::tasks_list) {
		m.total_task += task->CalculateMemoryUsed();
	}
	m.used = initial_mem_free-mem->GetHeapFreeSpace(HEAP_ANY);
	m.lost = m.used-m.total_task;

	return m;
}

void DumpMemory()
{
	auto m = CalculateMem();
	CLogger::Get()->Write("Memory", LogDebug, "Total memory: %d", m.total_memory);
	CLogger::Get()->Write("Memory", LogDebug, "Free memory: %d", m.free_memory);
	CLogger::Get()->Write("Memory", LogDebug, "Kernel size: %d", kernel_size);
	CLogger::Get()->Write("Memory", LogDebug, "Total task memory: %d", m.total_task);
	CLogger::Get()->Write("Memory", LogDebug, "Used memory: %d", m.used);
	CLogger::Get()->Write("Memory", LogDebug, "Lost memory: %d", m.lost);
	CLogger::Get()->Write("Memory", LogDebug, "Pre-boot memory: %d", pre_boot_memory);
	CLogger::Get()->Write("Memory", LogDebug, "---");
	for (auto& task: OSDTask::tasks_list) {
	}

}

