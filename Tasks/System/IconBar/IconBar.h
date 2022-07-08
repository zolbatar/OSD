#pragma once
#include <OS.h>
#include <WindowManager/WindowManager.h>

struct App
{
    OSDTask *task;
    std::string name;
    lv_img_dsc_t *icon;
};

struct Drive
{
    std::string name;
    std::string volume;
};

class IconBar : public OSDTask
{
  public:
    IconBar();

    void Run();
    static void RegisterApp(OSDTask *task, std::string name, lv_img_dsc_t *icon, lv_event_cb_t click_handler,
                            lv_event_cb_t press_handler);

  private:
    static lv_obj_t *icon_bar_cont;
    static lv_obj_t *icon_bar_left;
    static lv_obj_t *icon_bar_right;

    void AddDriveIcon(std::string name, std::string drive_name);
    static void DriveClickEventHandler(lv_event_t *e);
    static void TasksClickEventHandler(lv_event_t *e);
    static std::map<size_t, App> apps;
    static size_t index;
    std::list<Drive> drives;
};
