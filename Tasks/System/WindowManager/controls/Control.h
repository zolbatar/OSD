#pragma once
#include <OS.h>
#include <WindowManager/lvgl/lvgl.h>

class Control
{
  public:
    Control(OSDTask *task, lv_obj_t *parent);
    ~Control();

  protected:
    OSDTask *task;
    lv_obj_t *object;
    lv_obj_t *parent;
    std::list<Control> children;
};