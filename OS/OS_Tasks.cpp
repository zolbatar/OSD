#include "OS.h"
#include "../Tasks/System/WindowManager/WindowManager.h"
#include "../Library/StringLib.h"
#include "../Tasks/System/TasksWindow/TasksWindow.h"
#include "../Tasks/System/InputManager/InputManager.h"
#include "../Tasks/System/Filer/Filer.h"
#include "../Tasks/Editor/Editor.h"
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/logger.h>
#include <circle/usertimer.h>

extern WindowManager *gui;
extern unsigned rate;
CTask *OSDTask::boot_task;
OSDTask *OSDTask::current_task;
extern CUserTimer *UserTimer;
std::list<OSDTask *> OSDTask::tasks_list;
size_t OSDTask::task_id = 0;
bool OSDTask::yield_due = false;
OSDTask *OSDTask::task_override = nullptr;
OSDTask *OSDTask::task_active = NULL;

OSDTask *GetCurrentTask()
{
    if (OSDTask::GetOverride() != nullptr)
        return OSDTask::GetOverride();
    return OSDTask::current_task;
}

OSDTask::OSDTask() : CTask()
{
    fs.Init();
}

OSDTask::~OSDTask()
{
}

void OSDTask::SetNameAndAddToList()
{
    tasks_list.push_back(this);
}

void OSDTask::TerminateTask()
{
    terminate_requested = false;

    // Close window
    DirectMessage mess;
    mess.type = Messages::WM_CloseWindow;
    mess.source = this;
    CallGUIDirectEx(&mess);

    // If task is current?
    if (current_task == this)
    {
        current_task = NULL;
        InputManager::ClaimInput(NULL);
    }

    // Remove
    bool active = IsActive();
    tasks_list.remove(this);
    if (active)
    {
        SetActive(tasks_list.back());
    }
    Terminate();
}

void OSDTask::TaskTerminationHandler(CTask *ctask)
{
    OSDTask::current_task = (OSDTask *)ctask;
    switch (current_task->type)
    {
    case TaskType::DARIC: {
        auto dw = (DARICWindow *)ctask;
        delete dw;
        break;
    }
    case TaskType::Filer: {
        auto dw = (Filer *)ctask;
        delete dw;
        break;
    }
    case TaskType::TaskManager: {
        auto dw = (TasksWindow *)ctask;
        delete dw;
        break;
    }
    case TaskType::Editor: {
        auto dw = (Editor *)ctask;
        delete dw;
        break;
    }
    case TaskType::Other:
        delete current_task;
        break;
    }
}

void OSDTask::TaskSwitchHandler(CTask *ctask)
{
    if (ctask == boot_task)
    {
        UserTimer->Start(rate);
        return;
    }

    // OSD Task
    OSDTask::current_task = (OSDTask *)ctask;
    switch (current_task->priority)
    {
    case TaskPriority::System:
        UserTimer->Start(rate * 4096);
        break;
    case TaskPriority::User:
        UserTimer->Start(rate);
        break;
    }
    //	CLogger::Get()->Write("Daric", LogNotice, "Task focus: %s", ctask->GetName());
}

void OSDTask::ReceiveDirectEx(DirectMessage *m)
{
    assert(0);
}

void OSDTask::CallGUIDirectEx(DirectMessage *m)
{
    if (terminate_requested)
        TerminateTask();
    if (yield_due)
    {
        yield_due = false;
        Yield();
    }
    GetTask("@")->ReceiveDirectEx(m);
}

OSDTask *OSDTask::GetTask(const char *s)
{
    auto mScheduler = CScheduler::Get();
    auto task = mScheduler->GetTask(s);
    return (OSDTask *)task;
}

void OSDTask::Yield()
{
    yield_due = false;
    Strings.ClearTemporaryStrings();
    if (terminate_requested)
        TerminateTask();
    GUIStateCheck();
    auto mScheduler = CScheduler::Get();
    mScheduler->Yield();
}

void OSDTask::Sleep(int ms)
{
    Strings.ClearTemporaryStrings();
    if (terminate_requested)
        TerminateTask();
    GUIStateCheck();
    auto mScheduler = CScheduler::Get();
    mScheduler->MsSleep(ms);
}

void OSDTask::SetOverride(OSDTask *task)
{
    task_override = task;
}

void OSDTask::ClearOverride()
{
    task_override = nullptr;
}

OSDTask *OSDTask::GetOverride()
{
    return task_override;
}

void OSDTask::ReceiveKey(Key k)
{
    keyboard_queue.push(k);
}

void OSDTask::RequestTerminate()
{
    terminate_requested = true;
}

void OSDTask::Maximise()
{
    maximise_requested = false;
    CLogger::Get()->Write("Tasks Window", LogPanic, "Maximise not supported with this task type");
}

void OSDTask::Minimise()
{
    minimise_requested = false;
}

void OSDTask::MoveTaskToTop(OSDTask *task)
{
    tasks_list.remove(task);
    tasks_list.push_back(task);
}

void OSDTask::SetActive(OSDTask *task)
{
    // CLogger::Get()->Write("OSDTask", LogNotice, "Set active: %s", task->GetName());
    task_active = task;
    MoveTaskToTop(task);
    if (task != NULL && task->priority != TaskPriority::System)
    {
        InputManager::ClaimInput(task);
        auto window = (Window *)task->GUI.GetWindow();
        window->SetActive();
    }
}

bool OSDTask::IsActive()
{
    return task_active = this;
}
