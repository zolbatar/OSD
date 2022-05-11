#include "Control.h"

Control::~Control()
{
	lv_obj_del(object);
}