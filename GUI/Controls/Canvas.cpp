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
	OSDTask::LockVLGL("Canvas::Clear");
	lv_canvas_fill_bg(object, bg, LV_OPA_COVER);
	cursor_x = 0;
	cursor_y = 0;
	OSDTask::UnlockVLGL();
}

void Canvas::PlotPixel(int64_t x, int64_t y)
{
	OSDTask::LockVLGL("Canvas::PlotPixel");
	lv_canvas_set_px_color(object, x, y, fg);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	OSDTask::LockVLGL("Canvas::DrawLine");
	static lv_point_t points[2];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y2;
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = fg;
	line_dsc.width = w;
	lv_canvas_draw_line(object, points, 2, &line_dsc);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawTriangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	OSDTask::LockVLGL("Canvas::DrawTriangle");
	static lv_point_t points[4];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y2;
	points[2].x = x3;
	points[2].y = y3;
	points[3].x = x1;
	points[3].y = y1;
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = fg;
	line_dsc.width = w;
	lv_canvas_draw_line(object, points, 4, &line_dsc);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawTriangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	OSDTask::LockVLGL("Canvas::DrawTriangleFilled");
	static lv_point_t points[4];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y2;
	points[2].x = x3;
	points[2].y = y3;
	points[3].x = x1;
	points[3].y = y1;
	lv_draw_rect_dsc_t line_dsc;
	lv_draw_rect_dsc_init(&line_dsc);
	line_dsc.outline_color = fg;
	line_dsc.bg_color = bg;
	line_dsc.outline_width = w;
	lv_canvas_draw_polygon(object, points, 4, &line_dsc);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawCircle(int64_t x, int64_t y, int64_t r, int64_t w)
{
	OSDTask::LockVLGL("Canvas::DrawCircle");
	lv_draw_arc_dsc_t line_dsc;
	lv_draw_arc_dsc_init(&line_dsc);
	line_dsc.width = w;
	line_dsc.color = fg;
	lv_canvas_draw_arc(object, x, y, r, 0, 359, &line_dsc);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawCircleFilled(int64_t x, int64_t y, int64_t r, int64_t w)
{
	OSDTask::LockVLGL("Canvas::DrawCircleFilled");

	// Fill
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.radius = r;
	rect_dsc.bg_opa = LV_OPA_COVER;
	rect_dsc.bg_color = bg;
	lv_canvas_draw_rect(object, x-r, y-r, r*2, r*2, &rect_dsc);

	// Outline
	lv_draw_arc_dsc_t line_dsc;
	lv_draw_arc_dsc_init(&line_dsc);
	line_dsc.width = w;
	line_dsc.color = fg;
	lv_canvas_draw_arc(object, x, y, r, 0, 359, &line_dsc);

	OSDTask::UnlockVLGL();
}

void Canvas::DrawRectangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	OSDTask::LockVLGL("Canvas::Rectangle");
	static lv_point_t points[5];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y1;
	points[2].x = x2;
	points[2].y = y2;
	points[3].x = x1;
	points[3].y = y2;
	points[4].x = x1;
	points[4].y = y1;
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = fg;
	line_dsc.width = w;
	lv_canvas_draw_line(object, points, 5, &line_dsc);
	OSDTask::UnlockVLGL();
}

void Canvas::DrawRectangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	OSDTask::LockVLGL("Canvas::RectangleFilled");

	// Fill
	lv_draw_rect_dsc_t rect_dsc;
	lv_draw_rect_dsc_init(&rect_dsc);
	rect_dsc.bg_opa = LV_OPA_COVER;
	rect_dsc.bg_color = bg;
	lv_canvas_draw_rect(object, x1, y1, x2, y2, &rect_dsc);

	// Outline
	static lv_point_t points[5];
	points[0].x = x1;
	points[0].y = y1;
	points[1].x = x2;
	points[1].y = y1;
	points[2].x = x2;
	points[2].y = y2;
	points[3].x = x1;
	points[3].y = y2;
	points[4].x = x1;
	points[4].y = y1;
	lv_draw_line_dsc_t line_dsc;
	lv_draw_line_dsc_init(&line_dsc);
	line_dsc.color = fg;
	line_dsc.width = w;
	lv_canvas_draw_line(object, points, 5, &line_dsc);

	OSDTask::UnlockVLGL();
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
	OSDTask::LockVLGL("Canvas::PrintString");
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
	OSDTask::UnlockVLGL();
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
	OSDTask::LockVLGL("Canvas::ScrollUp");

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
	OSDTask::UnlockVLGL();
}

void Canvas::PrintTab(int64_t v)
{
	auto dest = v*size_h;

	// Invalid?
	if (dest>w)
		return;

	while (cursor_x<dest) {
		PrintString(" ");
	}
}




