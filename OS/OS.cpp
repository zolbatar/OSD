#include "OS.h"

#ifndef CLION
#include <circle/logger.h>
#else
#endif

extern size_t kernel_size;

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
	if (!bank) {
		auto ar = &refs_bank1[ref_count_bank1++];
		ar->m = m;
		ar->sz = size;
		ar->task = GetCurrentTask();
		if (ref_count_bank1==MAX_ALLOC_REF) {
			ProcessAllocList();
		}
	}
	else {
		auto ar = &refs_bank2[ref_count_bank2++];
		ar->m = m;
		ar->sz = size;
		ar->task = GetCurrentTask();
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

