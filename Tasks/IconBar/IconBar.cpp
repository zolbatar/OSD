#include "IconBar.h"
#include "../Filer/Filer.h"
#include "../TasksWindow/TasksWindow.h"

App* IconBar::app_clicked = NULL;
Drive* IconBar::drive_clicked = NULL;
unsigned IconBar::last_drive_click = 0;
unsigned IconBar::last_app_click = 0;

IconBar::IconBar()
{
	this->id = "@"+std::to_string(task_id++);
	this->name = "Icon Bar";
	type = TaskType::IconBar;
}

void IconBar::Run()
{
	SetOverride(this);
	SetNameAndAddToList();

	icon_bar_cont = lv_obj_create(lv_scr_act());
	lv_obj_set_size(icon_bar_cont, LV_PCT(100), 110);
	lv_obj_align(icon_bar_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_flex_flow(icon_bar_cont, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_cont, &style_iconbar, LV_STATE_DEFAULT);

	// Left side, drives
	icon_bar_left = lv_obj_create(icon_bar_cont);
	lv_obj_set_flex_grow(icon_bar_left, 1);
	lv_obj_set_size(icon_bar_left, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_set_flex_align(icon_bar_left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(icon_bar_left, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_left, &style_iconbar_inner, LV_STATE_DEFAULT);

	// Right side, apps and system
	icon_bar_right = lv_obj_create(icon_bar_cont);
	lv_obj_set_flex_grow(icon_bar_right, 1);
	lv_obj_set_size(icon_bar_right, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_set_flex_align(icon_bar_right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(icon_bar_right, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_right, &style_iconbar_inner, LV_STATE_DEFAULT);

	// Devices
	AddDriveIcon("SD Card", "Boot", ":BOOT");
	AddDriveIcon("Home", "Home", ":HOME");
	AddAppIcon("Sloth", "OS/D", "OS/D");

	ClearOverride();

	while (1) {
		Yield();
	}
}

void IconBar::AddDriveIcon(std::string name, std::string text, std::string drive_name)
{
	Drive drive;
	drive.name = name;
	drive.volume = drive_name;
	drives.push_back(std::move(drive));
	auto d = &drives.back();

	lv_obj_t* device_cont = lv_obj_create(icon_bar_left);
	lv_obj_set_size(device_cont, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_center(device_cont);
	lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(device_cont, &style_iconbar_inner, LV_STATE_DEFAULT);

	lv_obj_t* btn = lv_btn_create(device_cont);
	lv_obj_center(btn);
	lv_obj_set_size(btn, 64, 64);
	lv_obj_add_event_cb(btn, DriveClickEventHandler, LV_EVENT_SHORT_CLICKED, d);
	lv_obj_t* img = lv_img_create(btn);
	lv_obj_center(img);
	lv_img_set_src(img, WindowManager::GetIcon(name));
	lv_obj_add_style(btn, &style_iconbar_button, LV_STATE_DEFAULT);

	auto nam = lv_label_create(device_cont);
	lv_obj_center(nam);
	lv_label_set_text(nam, text.c_str());
}

void IconBar::AddAppIcon(std::string name, std::string text, std::string app_name)
{
	App app;
	app.name = app_name;
	apps.push_back(std::move(app));
	auto d = &apps.back();

	lv_obj_t* device_cont = lv_obj_create(icon_bar_right);
	lv_obj_set_size(device_cont, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_center(device_cont);
	lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(device_cont, &style_iconbar_inner, LV_STATE_DEFAULT);

	lv_obj_t* btn = lv_btn_create(device_cont);
	lv_obj_center(btn);
	lv_obj_set_size(btn, 64, 64);
	lv_obj_add_event_cb(btn, AppClickEventHandler, LV_EVENT_SHORT_CLICKED, d);
	lv_obj_t* img = lv_img_create(btn);
	lv_obj_center(img);
	lv_img_set_src(img, WindowManager::GetIcon(name));
	lv_obj_add_style(btn, &style_iconbar_button, LV_STATE_DEFAULT);

	auto nam = lv_label_create(device_cont);
	lv_obj_center(nam);
	lv_label_set_text(nam, text.c_str());
}

void IconBar::DriveClickEventHandler(lv_event_t* e)
{
	auto this_drive_clicked = (Drive*)e->user_data;
	if (drive_clicked!=NULL && drive_clicked->name==this_drive_clicked->name) {
		auto ticks = CTimer::Get()->GetClockTicks();
		auto diff = ticks-last_drive_click;
		if (diff<DOUBLE_CLICK_SPEED) {
			auto task = new Filer(drive_clicked->volume, "$");
			task->Start();
			drive_clicked = NULL;
			return;
		}
	}
	last_drive_click = CTimer::Get()->GetClockTicks();
	drive_clicked = this_drive_clicked;
}

void IconBar::AppClickEventHandler(lv_event_t* e)
{
	auto this_app_clicked = (App*)e->user_data;
	if (app_clicked!=NULL && app_clicked->name==this_app_clicked->name) {
		auto ticks = CTimer::Get()->GetClockTicks();
		auto diff = ticks-last_app_click;
		if (diff<DOUBLE_CLICK_SPEED) {
			if (app_clicked->name == "OS/D") {
				auto tasks = NEW TasksWindow(1100, 600, 768, 256);
				tasks->Start();
			}
			app_clicked = NULL;
			return;
		}
	}
	last_app_click = CTimer::Get()->GetClockTicks();
	app_clicked = this_app_clicked;
}