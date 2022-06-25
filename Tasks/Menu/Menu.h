#pragma once
#include <list>
#include "../../OS/OS.h"
#include <circle/logger.h>
#include "../WindowManager/WindowManager.h"

lv_obj_t* lv_mylist_add_btn(lv_obj_t* list, const char* txt, bool arrow, std::string shortcut);

enum class MenuItemType {
	SubMenu,
	Item,
	Separator
};

struct MenuItem {
	MenuItemType type;
	std::string v;
	std::string shortcut;
	lv_event_cb_t cb;
	void *user_data;
};

struct MenuDefinition {
	lv_obj_t* obj;
	std::list<MenuItem> items;
};

class Menu : public OSDTask {
public:
	Menu();
	void Run();

	static void OpenMenu(int x, int y, OSDTask* task, std::string title, MenuDefinition menu);
	static void CloseMenu();
private:
	static MenuDefinition menu;
	static lv_obj_t* block;
	static void ClickAwayEventHandler(lv_event_t* e);
};
