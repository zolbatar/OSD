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
	GetCurrentTask()->SendGUIMessage(Messages::Canvas_Clear, NULL);
}

void call_2D_colour(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	auto m = new Canvas_Colour();
	task->AddAllocation(sizeof(Canvas_Colour), m);
	m->colour = c;
	task->SendGUIMessage(Messages::Canvas_SetForegroundColour, m);
}

void call_2D_plot(int64_t x, int64_t y)
{
	auto task = GetCurrentTask();
	auto m = new Canvas_PlotPixel();
	task->AddAllocation(sizeof(Canvas_PlotPixel), m);
	m->x = x;
	m->y = y;
	task->SendGUIMessage(Messages::Canvas_PlotPixel, m);
}

void call_2D_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	auto task = GetCurrentTask();
	auto m = new Canvas_DrawLine();
	task->AddAllocation(sizeof(Canvas_DrawLine), m);
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	task->SendGUIMessage(Messages::Canvas_DrawLine, m);
}

