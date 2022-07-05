#include "IconBar.h"
#include "../Filer/Filer.h"
#include "../TasksWindow/TasksWindow.h"

lv_obj_t *IconBar::icon_bar_cont;
lv_obj_t *IconBar::icon_bar_left;
lv_obj_t *IconBar::icon_bar_right;
std::map<size_t, App> IconBar::apps;
size_t IconBar::index;

IconBar::IconBar()
{
    this->id = "@" + std::to_string(task_id++);
    this->name = "Icon Bar";
}

void IconBar::Run()
{
    SetOverride(this);
    SetNameAndAddToList();
    auto style_iconbar = ThemeManager::GetStyle(StyleAttribute::IconBar);
    auto style_iconbar_inner = ThemeManager::GetStyle(StyleAttribute::IconBarInner);

    icon_bar_cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(icon_bar_cont, LV_PCT(100), ThemeManager::GetConst(ConstAttribute::IconBarHeight));
    lv_obj_align(icon_bar_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(icon_bar_cont, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(icon_bar_cont, style_iconbar, LV_STATE_DEFAULT);

    // Left side, drives
    icon_bar_left = lv_obj_create(icon_bar_cont);
    lv_obj_set_flex_grow(icon_bar_left, 1);
    lv_obj_set_size(icon_bar_left, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_flex_align(icon_bar_left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(icon_bar_left, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(icon_bar_left, style_iconbar_inner, LV_STATE_DEFAULT);

    // Right side, apps and system
    icon_bar_right = lv_obj_create(icon_bar_cont);
    lv_obj_set_flex_grow(icon_bar_right, 1);
    lv_obj_set_size(icon_bar_right, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_flex_align(icon_bar_right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(icon_bar_right, LV_FLEX_FLOW_ROW_REVERSE);
    lv_obj_add_style(icon_bar_right, style_iconbar_inner, LV_STATE_DEFAULT);

    // Devices
    AddDriveIcon("Device/SDCard", ":BOOT");
    AddDriveIcon("Folder/Applications", ":APPS");
    AddDriveIcon("Folder/Home", ":HOME");
    RegisterApp(NULL, "Daric", WindowManager::GetIcon("Application/Sloth"), TasksClickEventHandler, NULL);

    ClearOverride();

    while (1)
    {
        Yield();
    }
}

void IconBar::AddDriveIcon(std::string name, std::string drive_name)
{
    Drive drive;
    drive.name = name;
    drive.volume = drive_name;
    drives.push_back(std::move(drive));
    auto d = &drives.back();

    auto style_iconbar_inner = ThemeManager::GetStyle(StyleAttribute::IconBarInner);
    auto style_focussed = ThemeManager::GetStyle(StyleAttribute::Focussed);
    auto style_iconbar_button = ThemeManager::GetStyle(StyleAttribute::IconBarButton);

    lv_obj_t *device_cont = lv_obj_create(icon_bar_left);
    lv_obj_set_size(device_cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_center(device_cont);
    lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(device_cont, style_iconbar_inner, LV_STATE_DEFAULT);
    lv_obj_add_style(device_cont, style_focussed, LV_STATE_FOCUSED);

    lv_obj_t *btn = lv_btn_create(device_cont);
    lv_obj_center(btn);
    lv_obj_set_size(btn, 64, 64);
    lv_obj_add_event_cb(btn, DriveClickEventHandler, LV_EVENT_SHORT_CLICKED, d);
    lv_obj_t *img = lv_img_create(btn);
    lv_obj_center(img);
    lv_img_set_src(img, WindowManager::GetIcon(name));
    lv_obj_add_style(btn, style_iconbar_button, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, ThemeManager::GetColour(ColourAttribute::IconBarBackground), LV_STATE_DEFAULT);
    lv_obj_add_style(btn, style_focussed, LV_STATE_FOCUSED);

    auto nam = lv_label_create(device_cont);
    lv_obj_center(nam);
    lv_label_set_text(nam, drive_name.c_str());
}

void IconBar::RegisterApp(OSDTask *task, std::string name, lv_img_dsc_t *icon, lv_event_cb_t click_handler,
                          lv_event_cb_t press_handler)
{
    App app;
    app.task = task;
    app.name = name;

    auto style_iconbar_inner = ThemeManager::GetStyle(StyleAttribute::IconBarInner);
    auto style_iconbar_button = ThemeManager::GetStyle(StyleAttribute::IconBarButton);

    lv_obj_t *device_cont = lv_obj_create(icon_bar_right);
    lv_obj_set_size(device_cont, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_center(device_cont);
    lv_obj_set_flex_align(device_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_flex_flow(device_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(device_cont, style_iconbar_inner, LV_STATE_DEFAULT);

    lv_obj_t *btn = lv_btn_create(device_cont);
    lv_obj_center(btn);
    lv_obj_set_size(btn, 64, 64);
    if (click_handler != NULL)
        lv_obj_add_event_cb(btn, click_handler, LV_EVENT_SHORT_CLICKED, task);
    if (press_handler != NULL)
        lv_obj_add_event_cb(btn, press_handler, LV_EVENT_PRESSED, task);
    lv_obj_t *img = lv_img_create(btn);
    lv_obj_center(img);
    app.icon = icon;
    lv_img_set_src(img, app.icon);
    lv_obj_add_style(btn, style_iconbar_button, LV_STATE_DEFAULT);

    auto nam = lv_label_create(device_cont);
    lv_obj_center(nam);
    lv_label_set_text(nam, name.c_str());

    apps.insert(std::make_pair(index++, std::move(app)));
}

void IconBar::DriveClickEventHandler(lv_event_t *e)
{
    auto drive_clicked = (Drive *)e->user_data;
    auto task = new Filer(drive_clicked->volume, "");
    task->Start();
}

void IconBar::TasksClickEventHandler(lv_event_t *e)
{
    auto app = (OSDTask *)e->user_data;
    auto tasks = new TasksWindow(1100, 600, 768, 256);
    tasks->Start();
}
