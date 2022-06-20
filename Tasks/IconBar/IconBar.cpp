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
	lv_obj_set_size(icon_bar_cont, LV_PCT(100), 300);
	lv_obj_align(icon_bar_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_set_flex_flow(icon_bar_cont, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_cont, &style_iconbar, LV_STATE_DEFAULT);

	// Left side, drives
	icon_bar_left = lv_obj_create(icon_bar_cont);
	lv_obj_set_size(icon_bar_left, LV_PCT(100), LV_PCT(100));
	lv_obj_align(icon_bar_left, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_flex_flow(icon_bar_left, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_left, &style_iconbar_inner, LV_STATE_DEFAULT);

	// Right side, apps and system
	icon_bar_right = lv_obj_create(icon_bar_cont);
	lv_obj_set_size(icon_bar_right, LV_PCT(100), LV_PCT(100));
	lv_obj_align(icon_bar_right, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_flex_flow(icon_bar_right, LV_FLEX_FLOW_ROW);
	lv_obj_add_style(icon_bar_right, &style_iconbar_inner, LV_STATE_DEFAULT);

	// Devices
	AddDriveIcon("Computer", ":SD");
	AddDriveIcon("SD Card", ":BOOT");
	AddDriveIcon("Home", ":HOME");

	ClearOverride();

	while (1) {
		Yield();
	}
}

void IconBar::AddDriveIcon(std::string name, std::string text)
{
	lv_obj_t* device_cont = lv_obj_create(icon_bar_left);
	lv_obj_set_size(device_cont, 160, LV_PCT(100));
	lv_obj_align(device_cont, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(device_cont, &style_iconbar, LV_STATE_DEFAULT);

	lv_obj_t* btn = lv_btn_create(device_cont);
	lv_obj_set_size(btn, 96, 96);
	lv_obj_t* img = lv_img_create(btn);
	lv_img_set_antialias(img, true);
	lv_img_set_src(img, WindowManager::GetIcon(name));
	lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(btn, &style_iconbar_button, LV_STATE_DEFAULT);

	auto title1 = lv_label_create(device_cont);
	lv_label_set_text(title1, text.c_str());

}