#pragma once
#include "../OS/OS.h"
#include "Control.h"

extern size_t body_font_height;
extern int dm;

class Canvas : public Control {
public:
	Canvas(lv_obj_t* parent, int w, int h);
	~Canvas();
	void Clear();
	void DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w);
	void DrawTriangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);
	void DrawTriangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);
	void PlotPixel(int64_t x, int64_t y);
	void SetFG(uint32_t fg);
	void SetBG(uint32_t bg);
	void PrintString(const char* s);
	void PrintNewLine();
	void PrintTab(int64_t v);
private:
	int w;
	int h;
	int cursor_x = 0;
	int cursor_y = 0;
	lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;

	lv_color_t fg = lv_color_white();
	lv_color_t bg = lv_color_black();
	uint8_t* buffer;
	lv_font_t* mono;
	int size_h = body_font_height/2*dm;
	int size_v = body_font_height*dm;

	void ScrollUp();
};
