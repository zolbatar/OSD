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

// Memory allocation
void* osd_malloc(size_t size);
void osd_free(void* m);
void* osd_realloc(void* ptr, size_t new_size);

// Strings
int64_t OS_Strings_Create(std::string& s);
std::string& OS_Strings_Get(int64_t idx);

// Tasks
OSDTask* GetCurrentTask();

