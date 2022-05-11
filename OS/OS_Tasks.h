#pragma once
#ifndef CLION
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#else

#include <thread>
#include <mutex>

#endif

#include <list>
#include <queue>
#include "OS_Messages.h"

#define DELETE delete
#ifdef CLION
#define NEW new
extern void DesktopStartup();
#else
#include <circle/new.h>
#include <circle/alloc.h>
#define NEW new(HEAP_ANY)
#endif

extern std::string string_error;
extern int64_t task_idx;
typedef void (*start)(void);

struct DataElement {
    ValueType type;
    int64_t iv = 0;
    double rv = 0.0;
    int64_t sv = 0;
};

class Message {
public:
    Message(Messages type, void *source, void *data)
            : type(type), source(source), data(data) {};
    Messages type;
    void *source; // This is an OSDTask
    void *data;
};

#ifndef CLION
class OSDTask : public CTask {
#else

class OSDTask {
#endif
public:
    OSDTask()
#ifndef CLION
    : CTask()
#endif
    {
        idx = task_idx++;
#ifdef CLION
#endif
    }

    ~OSDTask() {
        DELETE code;
        if (!allocations.empty()) {
            //		assert(false);
        }
        if (!strings.empty()) {
            //		assert(false);
        }
    }

#ifdef CLION

    virtual void Run();

#ifdef CLION

    void SetName(std::string name) {
        this->name = name;
        tasks.insert(std::make_pair(name, this));
        auto id = std::this_thread::get_id();
        thread_tasks.insert(std::make_pair(id, this));
    }

#endif

    void Start() {
        Run();
    }

    void WaitForTermination() {
    }

#endif

    int64_t GetIndex() {
        return task_idx;
    }

    void CompileSource(std::string code);
    void RunCode();
    void AddDataElement(DataElement de);
    DataElement *GetDataElement();
    void AddDataLabel(std::string s);
    void RestoreDataLabel(std::string s);
    int64_t AddString(std::string s);
    std::string &GetString(int64_t idx);
    void AddAllocation(size_t size, void *m);
    void FreeAllocation(void *m);

    void SetStart(start s) {
        exec = s;
    }

    void CreateCode(size_t code_size) {
        this->code_size = code_size;
        code = NEW uint8_t[code_size];
    }

    uint8_t *GetCode() { return code; }

    start GetExec() {
        return exec;
    }

    void SendMessage(Messages m, void *data, OSDTask *source);
    void SendGUIMessage(Messages m, void *data);

    void Yield() {
#ifndef CLION
        auto mScheduler = CScheduler::Get();
        mScheduler->Yield();
#else
        std::this_thread::yield();
#endif
    }

    void SetWindow(std::string id, void *w) {
        this->id = id;
        this->w = w;
    }

    std::string GetWindowID() { return id; }

    void *GetWindow() { return w; }

    bool IsExclusive() { return exclusive; }

    void Terminate() { terminate_requested = true; }

    bool TerminateRequested() { return terminate_requested; }

#ifdef CLION
    static std::map<std::string, OSDTask *> tasks;
    static std::map<std::thread::id, OSDTask *> thread_tasks;
#endif
protected:
    bool terminate_requested = false;
    OSDTask *GetTask(const char *s);
    std::queue<Message> message_queue;
    start exec;
    bool exclusive = false;
#ifdef CLION
    std::string name;
    std::mutex message_lock;
    std::mutex allocations_lock;
#endif
private:
    std::string id;
    void *w = NULL;
    int64_t idx;
    int64_t string_index = 0;
    size_t data_element_index = 0;
    std::map<void *, size_t> allocations;
    std::list<void *> to_be_freed;
    std::map<int64_t, std::string> strings;
    std::map<std::string, size_t> data_labels;
    std::vector<DataElement> data_elements;
    size_t code_size;
    uint8_t *code = NULL;
    static bool first_task;
};