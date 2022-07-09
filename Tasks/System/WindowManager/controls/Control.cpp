#include "Control.h"

Control::Control(OSDTask *task, lv_obj_t *parent)
{
    this->task = task;
    this->parent = parent;
}

Control::~Control()
{
    //    lv_obj_del(object);
}

lv_obj_t *Control::GetObject()
{
    return object;
}
