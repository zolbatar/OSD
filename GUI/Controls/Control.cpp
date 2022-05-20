#include "Control.h"
#include "../OS/OS.h"

Control::~Control()
{
	OSDTask::LockVLGL();
	lv_obj_del(object);
	OSDTask::UnlockVLGL();
}