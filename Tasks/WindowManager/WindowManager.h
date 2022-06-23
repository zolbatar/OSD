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

class FileType {
public:
	FileType(std::string extension, std::string icon, std::string application)
			:extension(extension), icon(icon), application(application) { }
	std::string extension;
	std::string icon;
	std::string application;
};

struct Icon {
	std::string filename;
	lv_img_dsc_t* image = NULL;
};

enum class MenuItemType {
	SubMenu,
	Item,
	Separator
};

struct MenuItem {
	MenuItemType type;
	std::string v;
	std::string shortcut;
	const char* icon = NULL;
};

class WindowManager : public OSDTask {
public:
	WindowManager();
	~WindowManager();
	void Run();
	void ReceiveDirectEx(DirectMessage* message);
	static lv_img_dsc_t* GetIcon(std::string name);
	static void LoadIcon(std::string filename, std::string name);
	static FileType* GetFileType(std::string type);
private:
	GuiCLVGL* clvgl;
	static std::map<std::string, Icon> icons;
	static std::map<std::string, FileType> types;
	lv_img_dsc_t* mouse_cursor;

	void SetupLVGLStyles();
	static lv_img_dsc_t* LoadPNG(std::string filename, int w, int h);
	static void ClickEventHandler(lv_event_t* e);
};

