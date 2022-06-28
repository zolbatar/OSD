#pragma once
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/new.h>
#include <circle/alloc.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/spinlock.h>
#include <circle/sched/mutex.h>
#include <string>
#include <array>
#include <vector>
#include <list>
#include "OS_Messages.h"
#include <chrono>
#include <set>
#include <queue>
#include <map>
#include "../Tokeniser/Types.h"
#include "../Tasks/FileManager/FileSystemObject.h"
#include "../Tasks/InputManager/InputManagerStruct.h"

extern "C"
{
#include "../Lightning/lightning.h"
#include "../Lightning/jit_private.h"
}

enum class TaskType
{
    Other,
    DARIC,
    TaskManager,
    Filer,
};

enum TaskPriority
{
    NoPreempt,
    Low,
    High
};

const size_t ALLOCATION_SIZE = 32768;

typedef void (*start)(void);

struct TaskAllocRef
{
    void *m = 0;
    size_t sz = 0;
};

struct DataElement
{
    ValueType type;
    int64_t iv = 0;
    double rv = 0.0;
    int64_t sv = 0;
};

struct DirectMessage
{
    Messages type;
    void *source; // This is an OSDTask
    void *data;
};

class OSDTask : public CTask
{
  public:
    OSDTask();
    ~OSDTask();
    size_t CalculateMemoryUsed();
    void SetNameAndAddToList();
    std::string LoadSource(std::string volume, std::string directory, std::string filename);
    bool CompileSource(std::string filename, std::string code);
    void RunCode(bool wait);
    void AddDataElement(DataElement de);
    DataElement *GetDataElement();
    void AddDataLabel(std::string s);
    void RestoreDataLabel(std::string s);

    // Strings
    void FreeString(int64_t index);
    int64_t AddString(std::string s);
    int64_t AddStringPermanent(std::string s);
    std::string &GetString(int64_t idx);
    void ClearTemporaryStrings();
    void MakeStringPermanent(int64_t idx);
    void FreeStringPermanent(int64_t idx);
    void SetConstantString(int64_t idx);

    // Malloc
    void AddAllocation(size_t size, void *m);
    bool FreeAllocation(void *m);

    // Input
    void ReceiveKey(Key k);

    // Code
    void SetStart(start s);
    void CreateCode(size_t code_size);
    uint8_t *GetCode();
    start GetExec();

    TaskPriority GetPriority()
    {
        return priority;
    }
    virtual void ReceiveDirectEx(DirectMessage *m);
    void CallGUIDirectEx(DirectMessage *m);
    void Yield();
    void Sleep(int ms);

    void SetWindow(void *w)
    {
        this->w = w;
    }

    static void TaskTerminationHandler(CTask *ctask);
    static void TaskSwitchHandler(CTask *ctask);

    std::string GetWindowID()
    {
        return id;
    }
    std::string GetWindowName()
    {
        return name;
    }
    void *GetWindow()
    {
        return w;
    }
    bool IsExclusive()
    {
        return exclusive;
    }
    void TerminateTask();
    void RequestTerminate();
    void RequestMaximise();
    void RequestMinimise();
    size_t GetStringCount()
    {
        return permanent_strings.size();
    }
    size_t GetStringCountTemporary()
    {
        return strings.size();
    }
    size_t GetAllocCount();

    static std::list<OSDTask *> tasks_list;
    static CTask *boot_task;
    static OSDTask *current_task;
    static OSDTask *focus_task;
    TaskType type = TaskType::Other;

    void SetID(std::string id)
    {
        this->id = id;
    }

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
    static bool yield_due;
    static OSDTask *GetOverride();
    static void SetOverride(OSDTask *task);
    static void ClearOverride();
    bool WaitAtEnd()
    {
        return wait;
    }
    int64_t GetT1()
    {
        return t1;
    }
    bool KeyboardQueueEmpty()
    {
        return keyboard_queue.empty();
    }

  protected:
    FileSystem fs;
    OSDTask *GetTask(const char *s);
    start exec;
    bool exclusive = false;
    int d_x;
    int d_y;
    int d_w;
    int d_h;
    static size_t task_id;
    std::string id;
    std::string name;
    TaskPriority priority = TaskPriority::High;
    bool terminate_requested = false;
    bool minimise_requested = false;
    bool maximise_requested = false;
    static OSDTask *task_override;
    virtual void Maximise();
    virtual void Minimise();
    std::queue<Key> keyboard_queue;

  private:
    bool wait;
    int64_t t1;
    void *w = NULL;
    int64_t idx;
    size_t framebuffer_memory = 0;
    int64_t string_index = 1;
    size_t data_element_index = 0;
    std::array<TaskAllocRef, ALLOCATION_SIZE> allocations;
    std::map<int64_t, std::string> strings;
    std::map<int64_t, std::string> permanent_strings;
    std::set<int64_t> constant_strings;
    std::map<std::string, size_t> data_labels;
    std::vector<DataElement> data_elements;
    size_t code_size;
    uint8_t *code = NULL;
    jit_state_t *_jit = NULL;
};

struct DARICString
{
    bool constant;
    std::string value;
};