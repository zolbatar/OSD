#pragma once
#include <list>
#include "../lvgl.h"
#include "../../Tasks/System/WindowManager/Style.h"

class Control
{
  public:
    ~Control();

  protected:
    lv_obj_t *object;
    lv_obj_t *parent;
    std::list<Control> children;
};
