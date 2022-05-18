#pragma once

#include "GUI.h"

class TasksWindow : public OSDTask {
public:
	TasksWindow(int x, int y, int w, int h);
	void Run();
private:
	lv_obj_t* cont_col = NULL;
	lv_coord_t* row_dsc = NULL;
	std::list<size_t> used_history;

	void UpdateTasks();
};
