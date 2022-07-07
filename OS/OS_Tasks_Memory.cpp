#include "OS_Tasks_Memory.h"

OSDTaskMemory::OSDTaskMemory()
{
    // To be safe, zero these out
    for (auto it = allocations.begin(); it != allocations.end(); ++it)
    {
        it->m = 0;
    }
}

OSDTaskMemory::~OSDTaskMemory()
{
    // Delete any allocations, FIXME
    /*	for (auto& alloc : allocations) {
            if (alloc.m!=0)
                free(alloc.m);
        }*/
}

size_t OSDTaskMemory::CalculateMemoryUsed()
{
    size_t r = 0;

    // Allocation
    for (auto &m : allocations)
    {
        if (m.m != 0)
            r += m.sz;
    }
    return r;
}

void OSDTaskMemory::AddAllocation(size_t size, void *m)
{
    // Scan for hole
    for (auto it = allocations.begin(); it != allocations.end(); ++it)
    {
        if (it->m == 0)
        {
            it->m = m;
            it->sz = size;
            return;
        }
    }
}

bool OSDTaskMemory::FreeAllocation(void *m)
{
    // Go backwards (as more likely to free the last allocation)
    for (auto it = allocations.rbegin(); it != allocations.rend(); ++it)
    {
        if (it->m == m)
        {
            it->m = 0;
            return true;
        }
    }
    //	assert(0);
    return false;
}

size_t OSDTaskMemory::GetAllocCount()
{
    size_t i = ALLOCATION_SIZE - 1;
    for (auto it = allocations.rbegin(); it != allocations.rend(); ++it)
    {
        if (it->m != 0)
        {
            return i;
        }
        i--;
    }
    return 0;
}
