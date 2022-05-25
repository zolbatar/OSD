#include "Canvas.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include "../../Tasks/FontManager/FontManager.h"

Canvas::Canvas(lv_obj_t* parent, int w, int h)
		:w(w), h(h)
{
	this->parent = parent;
	auto sz = (lv_img_cf_get_px_size(cf)*w)*h/8;
	buffer = NEW uint8_t[sz];
	object = lv_canvas_create(parent);
	lv_canvas_set_buffer(object, buffer, w, h, cf);
	lv_canvas_fill_bg(object, bg, LV_OPA_COVER);
	mono = font_mono;
}

Canvas::~Canvas()
{
	DELETE buffer;
}

void Canvas::Clear()
{
	lv_canvas_fill_bg(object, bg, LV_OPA_COVER);
	cursor_x = 0;
	cursor_y = 0;
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

void Canvas::PrintString(const char* s)
{
	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.font = mono;
	label_dsc.color = fg;
	char ss[2];
	ss[1] = 0;
	for (size_t i = 0; i<strlen(s); i++) {
		ss[0] = s[i];
		lv_canvas_draw_text(object, cursor_x, cursor_y, 1, &label_dsc, (const char*)&ss);
		cursor_x += size_h;

		if (cursor_x+(size_h)>w) {
			cursor_x = 0;
			cursor_y += body_font_height;

			// Move buffer
			if (cursor_y+size_v>h) {
				cursor_y -= body_font_height;
				ScrollUp();
			}
		}
	}
}

void Canvas::PrintNewLine()
{
	cursor_x = 0;
	cursor_y += size_v;

	// Move buffer
	if (cursor_y+size_v>h) {
		cursor_y -= size_v;
		ScrollUp();
	}
}

void Canvas::ScrollUp()
{
	// Move memory buffer up
	const int ss = (lv_img_cf_get_px_size(cf)*w)*h/8;
	const int bs = (lv_img_cf_get_px_size(cf)*w)*body_font_height/8;
	memcpy(buffer, buffer+bs, ss-bs);
	lv_obj_invalidate(object);

	// Clear new line
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.radius = 0;
	rect_dsc.bg_opa = LV_OPA_COVER;
	rect_dsc.bg_color = bg;
	lv_canvas_draw_rect(object, 0, h-body_font_height, w, body_font_height, &rect_dsc);
}

void Canvas::PrintTab(int64_t v)
{
	auto dest = v*size_h;

	// Invalid?
	if (dest > w)
		return;

	while (cursor_x<dest) {
		PrintString(" ");
	}
}




