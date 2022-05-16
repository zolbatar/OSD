#pragma once
#ifndef CLION
#include <circle/sched/scheduler.h>
#include <circle/sched/task.h>
#include <circle/sysconfig.h>
#include <circle/new.h>
#include <circle/alloc.h>
#define NEW new(HEAP_ANY)
#else

#include "../Concurrent/concurrentqueue.h"

#define NEW new
#endif
#define DELETE delete

#include <list>
#include "OS_Messages.h"

extern std::string string_error;
typedef void (*start)(void);

struct DataElement {
    ValueType type;
    int64_t iv = 0;
    double rv = 0.0;
    int64_t sv = 0;
};

class Message {
public:
    Message() {}

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
#else
#endif
    {
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

    void SetName(std::string name) {
        this->name = name;
        tasks.insert(std::make_pair(name, this));
    }

    void Start() {
        Run();
    }

    void WaitForTermination() {
    }

#endif

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
    std::string name;
#endif
    const size_t max_message_queue = 256;
protected:
    bool terminate_requested = false;
    OSDTask *GetTask(const char *s);
    moodycamel::ConcurrentQueue<Message> message_queue;
    start exec;
    bool exclusive = false;
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