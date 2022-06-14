#include "NativeCompiler.h"
#include "../Tasks/WindowManager/WindowManager.h"

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
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_Enable_Shadow;
	task->SendGUIMessage(std::move(mess));
	GetCurrentTask()->Yield();
}

void call_2D_clipon(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord2*)&mess.data;
	mess.type = Messages::Canvas_ClipOn;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_clipoff()
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_ClipOff;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_flip()
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_Flip;
	task->SendGUIMessage(std::move(mess));
	GetCurrentTask()->Yield();
}

void call_2D_cls()
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_Clear;
	task->SendGUIMessage(std::move(mess));
	GetCurrentTask()->Yield();
}

void call_2D_colour(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Colour*)&mess.data;
	mess.type = Messages::Canvas_SetForegroundColour;
	m->colour = c;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_colourbg(int64_t r, int64_t g, int64_t b)
{
	auto c = 0xFF000000+(r << 16)+(g << 8)+b;
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Colour*)&mess.data;
	mess.type = Messages::Canvas_SetBackgroundColour;
	m->colour = c;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_plot(int64_t x, int64_t y)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1*)&mess.data;
	mess.type = Messages::Canvas_PlotPixel;
	m->x = x;
	m->y = y;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_line(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord2W*)&mess.data;
	mess.type = Messages::Canvas_DrawLine;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_rectangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord2W*)&mess.data;
	mess.type = Messages::Canvas_Rectangle;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_rectanglefilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord2W*)&mess.data;
	mess.type = Messages::Canvas_RectangleFilled;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_triangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord3W*)&mess.data;
	mess.type = Messages::Canvas_Triangle;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	m->x3 = x3;
	m->y3 = y3;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_trianglefilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord3W*)&mess.data;
	mess.type = Messages::Canvas_TriangleFilled;
	m->x1 = x1;
	m->y1 = y1;
	m->x2 = x2;
	m->y2 = y2;
	m->x3 = x3;
	m->y3 = y3;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_circle(int64_t x, int64_t y, int64_t r, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1RW*)&mess.data;
	mess.type = Messages::Canvas_Circle;
	m->x = x;
	m->y = y;
	m->r = r;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_circlefilled(int64_t x, int64_t y, int64_t r, int64_t w)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1RW*)&mess.data;
	mess.type = Messages::Canvas_CircleFilled;
	m->x = x;
	m->y = y;
	m->r = r;
	m->w = w;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_text(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1S*)&mess.data;
	mess.type = Messages::Canvas_Text;
	m->x = x;
	m->y = y;
	m->s = s;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_textcentre(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1S*)&mess.data;
	mess.type = Messages::Canvas_TextCentre;
	m->x = x;
	m->y = y;
	m->s = s;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_textright(int64_t x, int64_t y, int64_t s)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (Coord1S*)&mess.data;
	mess.type = Messages::Canvas_TextRight;
	m->x = x;
	m->y = y;
	m->s = s;
	task->CallGUIDirect(std::move(mess));
}

void call_2D_font(int64_t ff, int64_t fs, int64_t size)
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	auto m = (SetFont*)&mess.data;
	mess.type = Messages::Canvas_SetFont;
	m->ff = ff;
	m->fs = fs;
	m->size = size;
	task->CallGUIDirect(std::move(mess));
}