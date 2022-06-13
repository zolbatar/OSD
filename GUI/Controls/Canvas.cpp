#include "Canvas.h"
#ifndef CLION
#include <circle/logger.h>
#include <circle/multicore.h>
#endif
#include "../../Tasks/FontManager/FontManager.h"

Canvas::Canvas(lv_obj_t* parent, int w, int h)
		:w(w), h(h)
{
	this->parent = parent;
	sz = (lv_img_cf_get_px_size(cf)*w)*h/8;
	buffer = NEW uint8_t[sz];
	GetCurrentTask()->AddFrameBufferMemory(sz);

	// First buffer
	firstbuffer = lv_canvas_create(parent);
	lv_canvas_set_buffer(firstbuffer, buffer, w, h, cf);
	lv_canvas_fill_bg(firstbuffer, bg, LV_OPA_COVER);

	object = firstbuffer;
}

Canvas::~Canvas()
{
	DELETE buffer;
	if (buffer_back!=nullptr) {
		DELETE buffer_back;
	}
	if (left_id!=0)
		lv_draw_mask_free_param(&left_id);
	if (right_id!=0)
		lv_draw_mask_free_param(&right_id);
	if (top_id!=0)
		lv_draw_mask_free_param(&top_id);
	if (bottom_id!=0)
		lv_draw_mask_free_param(&bottom_id);
}

void Canvas::EnableDoubleBuffering()
{
	double_buffered = true;
	buffer_back = NEW uint8_t[sz];
	GetCurrentTask()->AddFrameBufferMemory(sz);

	// Second buffer
	secondbuffer = lv_canvas_create(parent);
	lv_canvas_set_buffer(secondbuffer, buffer_back, w, h, cf);
	lv_canvas_fill_bg(secondbuffer, bg, LV_OPA_COVER);
	lv_obj_add_flag(secondbuffer, LV_OBJ_FLAG_HIDDEN);

	object = secondbuffer;
	which_buffer = false;
}

void Canvas::Flip()
{
	if (!which_buffer) {
		lv_obj_add_flag(firstbuffer, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(secondbuffer, LV_OBJ_FLAG_HIDDEN);
		object = firstbuffer;
	}
	else {
		lv_obj_add_flag(secondbuffer, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(firstbuffer, LV_OBJ_FLAG_HIDDEN);
		object = secondbuffer;
	}
	which_buffer = !which_buffer;
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

void Canvas::ClipOn(int64_t x1, int64_t y1, int64_t x2, int64_t y2)
{
	clip = true;
	clip_x1 = x1;
	clip_y1 = y1;
	clip_x2 = x2;
	clip_y2 = y2;

	lv_draw_mask_line_points_init(&line_mask_param_l, clip_x1, 0, clip_x1, h, LV_DRAW_MASK_LINE_SIDE_RIGHT);
	lv_draw_mask_line_points_init(&line_mask_param_r, clip_x2, 0, clip_x2, h, LV_DRAW_MASK_LINE_SIDE_LEFT);
	lv_draw_mask_line_points_init(&line_mask_param_t, 0, clip_y1, w, clip_y1, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
	lv_draw_mask_line_points_init(&line_mask_param_b, 0, clip_y2, w, clip_y2, LV_DRAW_MASK_LINE_SIDE_TOP);
}

void Canvas::ClipOff()
{
	clip = false;
	lv_draw_mask_free_param(&left_id);
	lv_draw_mask_free_param(&right_id);
	lv_draw_mask_free_param(&top_id);
	lv_draw_mask_free_param(&bottom_id);
	left_id = 0;
	right_id = 0;
	top_id = 0;
	bottom_id = 0;
}

void Canvas::SetupClip()
{
	if (clip) {
		left_id = lv_draw_mask_add(&line_mask_param_l, NULL);
		right_id = lv_draw_mask_add(&line_mask_param_r, NULL);
		top_id = lv_draw_mask_add(&line_mask_param_t, NULL);
		bottom_id = lv_draw_mask_add(&line_mask_param_b, NULL);
	}
}

void Canvas::ClearClip()
{
	if (clip) {
		lv_draw_mask_remove_id(left_id);
		lv_draw_mask_remove_id(right_id);
		lv_draw_mask_remove_id(top_id);
		lv_draw_mask_remove_id(bottom_id);
	}
}

void Canvas::DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	SetupClip();
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
	ClearClip();
}

void Canvas::DrawTriangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	SetupClip();
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
	ClearClip();
}

void Canvas::DrawTriangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w)
{
	SetupClip();
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
	ClearClip();
}

void Canvas::DrawCircle(int64_t x, int64_t y, int64_t r, int64_t w)
{
	SetupClip();
	lv_draw_arc_dsc_t line_dsc;
	lv_draw_arc_dsc_init(&line_dsc);
	line_dsc.width = w;
	line_dsc.color = fg;
	lv_canvas_draw_arc(object, x, y, r, 0, 359, &line_dsc);
	ClearClip();
}

void Canvas::DrawCircleFilled(int64_t x, int64_t y, int64_t r, int64_t w)
{
	SetupClip();

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

	ClearClip();
}

void Canvas::DrawRectangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	SetupClip();
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
	ClearClip();
}

void Canvas::DrawRectangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w)
{
	SetupClip();

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

	ClearClip();
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
	if (dest>w)
		return;

	while (cursor_x<dest) {
		PrintString(" ");
	}
}

void Canvas::DrawText(int64_t x, int64_t y, std::string s)
{
	auto task = GetCurrentTask();
	auto w = (Window*)task->GetWindow();
	assert(w!=NULL);

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.font = font;
	label_dsc.color = fg;
	lv_canvas_draw_text(object, x, y, w->GetContentWidth()-x, &label_dsc, s.c_str());
}

void Canvas::DrawTextCentre(int64_t x, int64_t y, std::string s)
{
	auto task = GetCurrentTask();
	auto w = (Window*)task->GetWindow();
	assert(w!=NULL);

	auto width = FontManager::GetWidth(font, s.c_str());
	auto pos = x-width/2;

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.font = font;
	label_dsc.color = fg;
	lv_canvas_draw_text(object, pos, y, w->GetContentWidth()-pos, &label_dsc, s.c_str());
}

void Canvas::DrawTextRight(int64_t x, int64_t y, std::string s)
{
	auto task = GetCurrentTask();
	auto w = (Window*)task->GetWindow();
	assert(w!=NULL);

	auto width = FontManager::GetWidth(font, s.c_str());
	auto pos = x-width;

	lv_draw_label_dsc_t label_dsc;
	lv_draw_label_dsc_init(&label_dsc);
	label_dsc.font = font;
	label_dsc.color = fg;
	lv_canvas_draw_text(object, pos, y, w->GetContentWidth()-pos, &label_dsc, s.c_str());
}

void Canvas::SetFont(std::string ff, std::string fs, int64_t size)
{
	font = FontManager::GetFontByNameStyleAndSize(ff, fs, size);
}



