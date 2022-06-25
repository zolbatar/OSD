#include "Menu.h"
#include "../../GUI/Window/LVGLWindow.h"

MenuDefinition Menu::menu;
lv_obj_t* Menu::block;

Menu::Menu()
{
	this->id = "Menu Manager";
	this->name = "Menu Manager";
	this->priority = TaskPriority::Low;
}

void Menu::Run()
{
	SetNameAndAddToList();
	while (1) {
		Yield();
	}
}

void Menu::OpenMenu(int x, int y, OSDTask* task, std::string title, MenuDefinition menu)
{
	WindowAttributes* wa = new WindowAttributes();
	wa->resizable = false;
	wa->fixed_size_content = true;

	// Create a big block so
	block = lv_btn_create(lv_scr_act());
	lv_obj_set_size(block, LV_PCT(100), LV_PCT(100));
	lv_obj_add_style(block, ThemeManager::GetStyle(StyleAttribute::MenuBackground), LV_STATE_DEFAULT);
	lv_obj_add_event_cb(block, ClickAwayEventHandler, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(block, ClickAwayEventHandler, LV_EVENT_LONG_PRESSED, NULL);

	// Menu window
	menu.obj = lv_mywin_create(lv_scr_act(), ThemeManager::GetConst(ConstAttribute::MenuHeaderHeight), wa);
	lv_obj_set_x(menu.obj, x);
	lv_obj_set_y(menu.obj, y);
	lv_obj_add_style(menu.obj, ThemeManager::GetStyle(StyleAttribute::Menu), LV_STATE_DEFAULT);
	auto h = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight)+(ThemeManager::GetConst(ConstAttribute::WindowBorderWidth)*3)+8;

	auto content = lv_mywin_get_content(menu.obj);
	lv_obj_add_style(content, ThemeManager::GetStyle(StyleAttribute::MenuContainer), LV_STATE_DEFAULT);
	lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

	// Header
	lv_mywin_add_title(menu.obj, title.c_str());
	auto header = lv_mywin_get_header(menu.obj);
	lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowHeader), LV_STATE_DEFAULT);
	lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::MenuHeader), LV_STATE_DEFAULT);

	// Create a list
	auto cont_list = lv_list_create(content);
	lv_obj_clear_flag(cont_list, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(cont_list, 160, LV_SIZE_CONTENT);
	lv_obj_center(cont_list);
	lv_obj_add_style(cont_list, ThemeManager::GetStyle(StyleAttribute::MenuContainer), LV_STATE_DEFAULT);

	// Items
	for (auto& mi : menu.items) {
		switch (mi.type) {
			case MenuItemType::SubMenu: {
				auto btn = lv_mylist_add_btn(cont_list, mi.v.c_str(), true, mi.shortcut);
				lv_obj_add_style(btn, ThemeManager::GetStyle(StyleAttribute::MenuItem), LV_STATE_DEFAULT);
				lv_obj_add_event_cb(btn, NULL, LV_EVENT_CLICKED, NULL);
				break;
			}
			case MenuItemType::Item: {
				auto btn = lv_mylist_add_btn(cont_list, mi.v.c_str(), false, mi.shortcut);
				lv_obj_add_style(btn, ThemeManager::GetStyle(StyleAttribute::MenuItem), LV_STATE_DEFAULT);
				lv_obj_add_event_cb(btn, mi.cb, LV_EVENT_CLICKED, mi.user_data);
				break;
			}
			case MenuItemType::Separator: {
				auto label = lv_label_create(cont_list);
				lv_label_set_text(label, "");
				lv_obj_set_size(label, LV_PCT(100), 1);
				lv_obj_add_style(label, ThemeManager::GetStyle(StyleAttribute::MenuSeparator), LV_STATE_DEFAULT);
				break;
			}
			default:
				break;
		}
	}

	// Set menu height
	lv_obj_update_layout(cont_list);
	lv_obj_set_width(menu.obj, lv_obj_get_width(cont_list)+8);
	lv_obj_set_height(menu.obj, lv_obj_get_height(cont_list)+h);

	// Save for events
	Menu::menu = menu;
}

lv_obj_t* lv_mylist_add_btn(lv_obj_t* list, const char* txt, bool arrow, std::string shortcut)
{
	lv_obj_t* obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
	lv_obj_class_init_obj(obj);
	lv_obj_set_size(obj, LV_PCT(100), ThemeManager::GetConst(ConstAttribute::HeaderFontSize));
	lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
	if (txt) {
		lv_obj_t* label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
		lv_obj_set_flex_grow(label, 1);
	}
	if (arrow) {
		lv_obj_t* img = lv_img_create(obj);
		lv_img_set_src(img, LV_SYMBOL_MENURIGHT);
		lv_obj_set_size(img, 16, 16);
		lv_img_set_offset_y(img, 4);
		lv_obj_add_style(img, ThemeManager::GetStyle(StyleAttribute::FontSymbol), LV_STATE_DEFAULT);
	}
	if (!shortcut.empty()) {
		lv_obj_t* label = lv_label_create(obj);
		lv_label_set_text(label, shortcut.c_str());
		lv_obj_set_flex_grow(label, 0);
		lv_obj_add_style(label, ThemeManager::GetStyle(StyleAttribute::MenuItemBold), LV_STATE_DEFAULT);
	}

	return obj;
}

void Menu::CloseMenu() {
	lv_obj_del(block);
	lv_obj_del(menu.obj);
}

void Menu::ClickAwayEventHandler(lv_event_t* e)
{
	lv_obj_del(block);
	lv_obj_del(menu.obj);
}



