#pragma once
#include "../WindowManager/WindowManager.h"

class Editor : public OSDTask {
public:
	Editor(int x, int y, int w, int h);

	void Run();
private:
};
