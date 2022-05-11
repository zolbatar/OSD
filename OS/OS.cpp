#include "OS.h"

#ifndef CLION
#include <circle/new.h>
#else
#endif

void OS_Init()
{
}

void* osd_malloc(size_t size)
{
#ifndef CLION
	auto m = CMemorySystem::HeapAllocate(size, HEAP_ANY);
#else
	auto m = malloc(size);
#endif
	GetCurrentTask()->AddAllocation(size, m);
	return m;
}

void osd_free(void* m)
{
	GetCurrentTask()->FreeAllocation(m);
#ifndef CLION
	CMemorySystem::HeapFree (m);
#else
	free(m);
#endif
}
