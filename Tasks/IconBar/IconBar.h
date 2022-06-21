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

struct App {
	std::string name;
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
	static unsigned last_drive_click;
	static App* app_clicked;
	static unsigned last_app_click;

	void AddDriveIcon(std::string name, std::string text, std::string drive_name);
	void AddAppIcon(std::string name, std::string text, std::string app_name);
	static void DriveClickEventHandler(lv_event_t* e);
	static void AppClickEventHandler(lv_event_t* e);
	std::list<App> apps;
	std::list<Drive> drives;
};

