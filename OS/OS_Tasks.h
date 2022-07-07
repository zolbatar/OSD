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
#include <list>
#include <chrono>
#include <queue>
#include "OS_Messages.h"
#include "OS_Tasks_Strings.h"
#include "OS_Tasks_Data.h"
#include "OS_Tasks_Code.h"
#include "OS_Tasks_GUI.h"
#include "OS_Tasks_Memory.h"
#include "../Compiler/Tokeniser/Types.h"
#include "../Tasks/System/FileManager/FileSystemObject.h"
#include "../Tasks/System/InputManager/InputManagerStruct.h"

enum class TaskType
{
    Other,
    DARIC,
    TaskManager,
    Filer,
    Editor,
};

enum TaskPriority
{
    System,
    User
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
    FileSystem fs;
    OSDTaskStrings Strings;
    OSDTaskData Data;
    OSDTaskCode Code;
    OSDTaskGUI GUI;
    OSDTaskMemory Memory;

    void SetNameAndAddToList();
    void ReceiveKey(Key k);

    void SetID(std::string id)
    {
        this->id = id;
    }

    std::string GetID()
    {
        return id;
    }

    TaskPriority GetPriority()
    {
        return priority;
    }

    bool KeyboardQueueEmpty()
    {
        return keyboard_queue.empty();
    }

    void GUIStateCheck()
    {
        if (maximise_requested)
            Maximise();
        if (minimise_requested)
            Minimise();
    }

    void RequestMaximise()
    {
        maximise_requested = true;
    }

    void RequestMinimise()
    {
        minimise_requested = true;
    }

    virtual void ReceiveDirectEx(DirectMessage *m);
    void CallGUIDirectEx(DirectMessage *m);
    void Yield();
    void Sleep(int ms);
    void TerminateTask();
    void RequestTerminate();
    size_t GetAllocCount();
    static void TaskTerminationHandler(CTask *ctask);
    static void TaskSwitchHandler(CTask *ctask);
    static OSDTask *GetOverride();
    static void SetOverride(OSDTask *task);
    static void ClearOverride();

    static std::list<OSDTask *> tasks_list;
    static CTask *boot_task;
    static OSDTask *current_task;
    TaskType type = TaskType::Other;
    static bool yield_due;

  protected:
    bool minimise_requested = false;
    bool maximise_requested = false;
    OSDTask *GetTask(const char *s);
    static size_t task_id;
    std::string id;
    TaskPriority priority = TaskPriority::User;
    bool terminate_requested = false;
    static OSDTask *task_override;
    virtual void Maximise();
    virtual void Minimise();
    std::queue<Key> keyboard_queue;

  private:
    int64_t idx;
};