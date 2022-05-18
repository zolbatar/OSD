#pragma once

#include "GUI.h"

class TasksWindow : public OSDTask {
public:
    TasksWindow(int x, int y, int w, int h);

    void Run();
private:
    std::list<size_t> used_history;

    void UpdateTasks();
};
