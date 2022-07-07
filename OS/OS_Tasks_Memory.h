#pragma once
#include <cstddef>
#include <array>

const size_t ALLOCATION_SIZE = 32768;

struct TaskAllocRef
{
    void *m = 0;
    size_t sz = 0;
};

class OSDTaskMemory
{
  public:
    OSDTaskMemory();
    ~OSDTaskMemory();
    void AddAllocation(size_t size, void *m);
    bool FreeAllocation(void *m);
    size_t CalculateMemoryUsed();
    size_t GetAllocCount();

    void AddFrameBufferMemory(size_t t)
    {
        framebuffer_memory += t;
    }

    void RemoveFrameBufferMemory(size_t t)
    {
        framebuffer_memory -= t;
    }

    size_t GetFrameBufferMemory()
    {
        return framebuffer_memory;
    }

  private:
    size_t framebuffer_memory = 0;
    std::array<TaskAllocRef, ALLOCATION_SIZE> allocations;
};