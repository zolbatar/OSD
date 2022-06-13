#pragma once
#include "WindowManager/WindowManager.h"

class TasksWindow : public OSDTask {
public:
	TasksWindow(int x, int y, int w, int h);

	void Run();
	void UpdateGUI();
private:
	const int HISTORY_SIZE = 64;
	std::list<size_t> used_history;

};
