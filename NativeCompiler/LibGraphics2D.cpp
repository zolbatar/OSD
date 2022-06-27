#include "NativeCompiler.h"
#include "../Tasks/WindowManager/WindowManager.h"

void call_2D_mode(int64_t x, int64_t y)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_Mode;
	Coord1 m;
	mess.data = &m;
	m.x = x;
	m.y = y;
	task->CallGUIDirectEx(&mess);
}

int64_t call_2D_screenwidth()
{
	auto w = (Window*)GetCurrentTask()->GetWindow();
	assert(w!=NULL);
	return w->GetContentWidth();
}

int64_t call_2D_screenheight()
{
	auto w = (Window*)GetCurrentTask()->GetWindow();
	assert(w!=NULL);
	return w->GetContentHeight();
}

void call_2D_shadow()
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_Enable_Shadow;
	task->CallGUIDirectEx(&mess);
}

void call_2D_clipon(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord2 m;
	mess.data = &m;
	mess.type = Messages::Canvas_ClipOn;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	task->CallGUIDirectEx(&mess);
}

void call_2D_clipoff()
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_ClipOff;
	task->CallGUIDirectEx(&mess);
}

void call_2D_flip()
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_Flip;
	task->CallGUIDirectEx(&mess);
}

void call_2D_cls()
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_Clear;
	task->CallGUIDirectEx(&mess);
}

void call_2D_colour(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Colour m;
	mess.data = &m;
	mess.type = Messages::Canvas_SetForegroundColour;
	m.colour = c;
	task->CallGUIDirectEx(&mess);
}

void call_2D_colourbg(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Colour m;
	mess.data = &m;
	mess.type = Messages::Canvas_SetBackgroundColour;
	m.colour = c;
	task->CallGUIDirectEx(&mess);
}

void call_2D_plot(int64_t x, int64_t y)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PlotPixel;
	Coord1 m;
	mess.data = &m;
	m.x = x;
	m.y = y;
	task->CallGUIDirectEx(&mess);
}

void call_2D_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord2W m;
	mess.data = &m;
	mess.type = Messages::Canvas_DrawLine;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_rectangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord2W m;
	mess.data = &m;
	mess.type = Messages::Canvas_Rectangle;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_rectanglefilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord2W m;
	mess.data = &m;
	mess.type = Messages::Canvas_RectangleFilled;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_triangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord3W m;
	mess.data = &m;
	mess.type = Messages::Canvas_Triangle;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.x3 = x3;
	m.y3 = y3;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_trianglefilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord3W m;
	mess.data = &m;
	mess.type = Messages::Canvas_TriangleFilled;
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.x3 = x3;
	m.y3 = y3;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_circle(int64_t x, int64_t y, int64_t r, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord1RW m;
	mess.data = &m;
	mess.type = Messages::Canvas_Circle;
	m.x = x;
	m.y = y;
	m.r = r;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_circlefilled(int64_t x, int64_t y, int64_t r, int64_t w)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord1RW m;
	mess.data = &m;
	mess.type = Messages::Canvas_CircleFilled;
	m.x = x;
	m.y = y;
	m.r = r;
	m.w = w;
	task->CallGUIDirectEx(&mess);
}

void call_2D_text(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord1S m;
	mess.data = &m;
	mess.type = Messages::Canvas_Text;
	m.x = x;
	m.y = y;
	m.s = s;
	task->CallGUIDirectEx(&mess);
	task->ClearTemporaryStrings();
}

void call_2D_textcentre(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord1S m;
	mess.data = &m;
	mess.type = Messages::Canvas_TextCentre;
	m.x = x;
	m.y = y;
	m.s = s;
	task->CallGUIDirectEx(&mess);
	task->ClearTemporaryStrings();
}

void call_2D_textright(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	Coord1S m;
	mess.data = &m;
	mess.type = Messages::Canvas_TextRight;
	m.x = x;
	m.y = y;
	m.s = s;
	task->CallGUIDirectEx(&mess);
	task->ClearTemporaryStrings();
}

void call_2D_font(int64_t ff, int64_t fs, int64_t size)
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	SetFont m;
	mess.data = &m;
	mess.type = Messages::Canvas_SetFont;
	m.ff = ff;
	m.fs = fs;
	m.size = size;
	task->CallGUIDirectEx(&mess);
	task->ClearTemporaryStrings();
}