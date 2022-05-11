#pragma once
#include "../OS/OS.h"
#include "../GUI/Controls/Control.h"
#include "../GUI/Controls/Canvas.h"
#include "../GUI/Window.h"
#include "../GUI/lvgl.h"
#include "../GUI/Style.h"
#include <map>

const int COL_BG = 0x404040;

class GUI : public OSDTask {
public:
	GUI();
	~GUI();
	void Run();

private:
	GuiCLVGL* clvgl;
	std::map<std::string, Window*> windows;
};

