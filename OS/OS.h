#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include "../Tokeniser/Types.h"
#include "OS_Tasks.h"

#define DELETE delete
#ifdef CLION
#define NEW new
#else
#include <circle/new.h>
#include <circle/alloc.h>
#define NEW new(HEAP_ANY)
#endif

void OS_Init();

// Strings
void OS_Strings_Free(int64_t index);
int64_t OS_Strings_Create(std::string& s);
std::string& OS_Strings_Get(int64_t idx);
int64_t OS_Strings_CreatePermanent(std::string& s);
void OS_Strings_MakePermanent(int64_t s);
void OS_Strings_FreePermanent(int64_t s);

// Tasks
OSDTask* GetCurrentTask();

struct MemorySummary {
	size_t total_memory = 0;
	size_t free_memory = 0;
	size_t total_task = 0;
	size_t used = 0;
	size_t lost = 0;
};

void CalculateMem(MemorySummary* m);
void DumpMemory();