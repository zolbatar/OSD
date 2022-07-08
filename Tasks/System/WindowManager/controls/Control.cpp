#include "Control.h"

Control::Control(OSDTask *task, lv_obj_t *parent) : task(task), parent(parent)
{
}

Control::~Control()
{
    lv_obj_del(object);
}