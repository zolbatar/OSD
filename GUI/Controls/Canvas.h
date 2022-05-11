#pragma once
#include "../OS/OS.h"
#include "Control.h"

class Canvas : public Control {
public:
	Canvas(lv_obj_t* parent, int w, int h);
	~Canvas();
	void Clear();
	void DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2);
	void PlotPixel(int64_t x, int64_t y);
	void SetFG(uint32_t fg);
	void SetBG(uint32_t bg);
private:
	int w;
	int h;

	lv_color_t fg = lv_color_white();
	lv_color_t bg = lv_color_black();
	uint8_t* buffer;
};
