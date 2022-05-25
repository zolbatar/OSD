#pragma once
#include "../../OS/OS.h"
#include "../../GUI/Controls/Control.h"
#include "../../GUI/Controls/Canvas.h"
#include "../../GUI/Window.h"
#include "../../GUI/lvgl.h"
#include "../../GUI/Style.h"
#include "../DARICWindow.h"
#include <map>
#include <memory>

class WindowManager : public OSDTask {
public:
	WindowManager();
	~WindowManager();
	void Run();

private:
	GuiCLVGL* clvgl;

	void DesktopStartup();
};

