#pragma once
#include "GUI.h"

class TasksWindow : public OSDTask {
public:
	TasksWindow(int x, int y, int w, int h);
	void Run();
private:
	lv_obj_t* cont;

	void UpdateTasks();
};
