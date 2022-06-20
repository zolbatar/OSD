#pragma once
#include "../../OS/OS.h"
#include <circle/logger.h>
#include "../../GUI/Window/LVGLWindow.h"
#include "../../GUI/Style.h"
#include "../WindowManager/WindowManager.h"

class IconBar : public OSDTask {
public:
	IconBar();

	void Run();
private:
	lv_obj_t* icon_bar_cont;
	lv_obj_t* icon_bar_left;
	lv_obj_t* icon_bar_right;

	void AddDriveIcon(std::string name, std::string text);
};

