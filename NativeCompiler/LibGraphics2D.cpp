#include "NativeCompiler.h"
#include "../OS/OS.h"
#include "../Tasks/GUI.h"

int64_t call_2D_screenwidth()
{
	auto task = GetCurrentTask();
	auto w = (Window*)task->GetWindow();
	return w->GetContentWidth();
}

int64_t call_2D_screenheight()
{
	auto w = (Window*)GetCurrentTask()->GetWindow();
	return w->GetContentHeight();
}

void call_2D_cls()
{
	auto task = GetCurrentTask();
	auto mess = task->SendGUIMessage();
	mess->source = task;
	mess->type = Messages::Canvas_Clear;
}

void call_2D_colour(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	auto mess = task->SendGUIMessage();
	auto m = (Canvas_Colour*)&mess->data;
	mess->source = task;
	mess->type = Messages::Canvas_SetForegroundColour;
	m->colour = c;
}

void call_2D_colourbg(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	auto mess = task->SendGUIMessage();
	auto m = (Canvas_Colour*)&mess->data;
	mess->source = task;
	mess->type = Messages::Canvas_SetBackgroundColour;
	m->colour = c;
}

void call_2D_plot(int64_t x, int64_t y)
{
	auto task = GetCurrentTask();
	auto mess = task->SendGUIMessage();
	auto m = (Canvas_PlotPixel*)&mess->data;
	mess->source = task;
	mess->type = Messages::Canvas_PlotPixel;
	m->x = x;
	m->y = y;
}

void call_2D_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	auto task = GetCurrentTask();
	auto mess = task->SendGUIMessage();
	auto m = (Canvas_DrawLine*)&mess->data;
	mess->source = task;
	mess->type = Messages::Canvas_DrawLine;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
}

