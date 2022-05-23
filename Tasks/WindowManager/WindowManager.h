#pragma once
#include "../../OS/OS.h"
#include "../../GUI/Controls/Control.h"
#include "../../GUI/Controls/Canvas.h"
#include "../../GUI/Window.h"
#include "../../GUI/lvgl.h"
#include "../../GUI/Style.h"
#include <map>

class WindowManager : public OSDTask {
public:
	WindowManager();
	~WindowManager();
	void Run();

private:
	GuiCLVGL* clvgl;
	std::map<std::string, Window*> windows;
};

