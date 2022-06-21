#pragma once
#include <list>
#include "../../OS/OS.h"
#include <circle/logger.h>
#include <circle/timer.h>
#include "../../GUI/Window/LVGLWindow.h"
#include "../../GUI/Style.h"
#include "../WindowManager/WindowManager.h"

struct Drive {
	std::string name;
	std::string volume;
};

class IconBar : public OSDTask {
public:
	IconBar();

	void Run();
private:
	lv_obj_t* icon_bar_cont;
	lv_obj_t* icon_bar_left;
	lv_obj_t* icon_bar_right;
	static Drive* drive_clicked;
	static unsigned last_click;

	void AddDriveIcon(std::string name, std::string text, std::string drive_name);
	void AddAppIcon(std::string name, std::string text);
	static void DriveClickEventHandler(lv_event_t* e);
	std::list<Drive> drives;
};

