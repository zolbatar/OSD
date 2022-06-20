#pragma once
#include "../../OS/OS.h"
#include "../../GUI/Controls/Control.h"
#include "../../GUI/Controls/Canvas.h"
#include "../../GUI/Window/Window.h"
#include "../../GUI/lvgl.h"
#include "../../GUI/Style.h"
#include "../DARICWindow.h"
#include <map>
#include <memory>

enum class MenuItemType {
	Item,
	Separator
};

struct MenuItem {
	MenuItemType type;
	std::string v;
};

struct Menu {
	std::list<MenuItem> items;
};

class WindowManager : public OSDTask {
public:
	WindowManager();
	~WindowManager();
	void Run();
	void DesktopStartup();
	void ReceiveDirectEx(DirectMessage* message);
	static lv_img_dsc_t* GetIcon(std::string name) { return &icons.find(name)->second; }
private:
	GuiCLVGL* clvgl;
	lv_obj_t* menu_win = NULL;
	Menu* menu = NULL;
	static std::map<std::string, lv_img_dsc_t> icons;

	void SetupLVGLStyles();
	void LoadIcon(std::string filename, std::string name);
	void SetupStyleBackground();
	void SetupIcons();
	static void ClickEventHandler(lv_event_t* e);
	void CreateMenu(int x, int y, OSDTask* task, std::string title, Menu* menu);
};

