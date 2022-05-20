#include "Control.h"
#include "../OS/OS.h"

Control::~Control()
{
	lv_obj_del(object);
}