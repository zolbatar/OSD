#include "Canvas.h"

Canvas::Canvas(lv_obj_t* parent, int w, int h)
		:w(w), h(h)
{
	this->parent = parent;
	auto cf = LV_IMG_CF_TRUE_COLOR;
	auto sz = (lv_img_cf_get_px_size(cf)*w)/8*h;
	buffer = NEW uint8_t[sz];
	object = lv_canvas_create(parent);
	GetCurrentTask()->AddAllocation(sz, buffer);
	lv_canvas_set_buffer(object, buffer, w, h, cf);
	lv_canvas_fill_bg(object, bg, LV_OPA_COVER);
}

Canvas::~Canvas()
{
	GetCurrentTask()->FreeAllocation(buffer);
	lv_obj_clean(object);
}

void Canvas::Clear()
{
	lv_canvas_fill_bg(object, bg, LV_OPA_COVER);
}

void Canvas::PlotPixel(int64_t x, int64_t y)
{
	lv_canvas_set_px_color(object, x, y, fg);
}

void Canvas::DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	static lv_point_t points[2];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y2;
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = fg;
	lv_canvas_draw_line(object, points, 2, &line_dsc);
}

void Canvas::SetFG(uint32_t fg)
{
	this->fg = lv_color_hex(fg);
}

void Canvas::SetBG(uint32_t bg)
{
	this->bg = lv_color_hex(bg);
}



