#include "IconBar.h"

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
	lv_obj_set_size(icon_bar_cont, LV_PCT(100), 128);
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
	AddDriveIcon("Boot", ":SD");
	AddDriveIcon("SD Card", ":BOOT");
	AddDriveIcon("RAM", ":RAM");
	AddDriveIcon("Home", ":HOME");

	ClearOverride();

	while (1) {
		Yield();
	}
}

void IconBar::AddDriveIcon(std::string name, std::string text)
{
	lv_obj_t* device_cont = lv_obj_create(icon_bar_left);
	lv_obj_set_size(device_cont, LV_SIZE_CONTENT, LV_PCT(100));
	lv_obj_align(device_cont, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(device_cont, &style_iconbar_inner, LV_STATE_DEFAULT);

	lv_obj_t* btn = lv_btn_create(device_cont);
	lv_obj_center(btn);
	lv_obj_set_size(btn, 64, 64);
	lv_obj_t* img = lv_img_create(btn);
	lv_obj_center(img);
	lv_img_set_antialias(img, true);
	lv_img_set_src(img, WindowManager::GetIcon(name));
	lv_obj_add_style(btn, &style_iconbar_button, LV_STATE_DEFAULT);

	auto nam = lv_label_create(device_cont);
	lv_obj_center(nam);
	lv_label_set_text(nam, text.c_str());
}