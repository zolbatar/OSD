#pragma once
#include "../OS/OS.h"
#include "Control.h"
#include "../../Tasks/WindowManager/Style.h"

class Canvas : public Control {
public:
	Canvas(OSDTask* task, lv_obj_t* parent, int w, int h);
	~Canvas();
	int64_t GetContentWidth();
	int64_t GetContentHeight();
	int GetSizeH() { return size_h; }
	int GetSizeV() { return size_v; }
	int GetW() { return w; }
	int GetH() { return h; }
	lv_obj_t* GetFirstBuffer() { return firstbuffer; }

	void Render();
	void SetupClip();
	void ClearClip();
	void EnableDoubleBuffering();
	void Flip();
	void Mode(int64_t w, int64_t h);
	void Clear();
	void ClipOn(int64_t x1, int64_t y1, int64_t x2, int64_t y2);
	void ClipOff();
	void DrawLine(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w);
	void DrawTriangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);
	void DrawTriangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t x3, int64_t y3, int64_t w);
	void DrawCircle(int64_t x, int64_t y, int64_t r, int64_t w);
	void DrawCircleFilled(int64_t x, int64_t y, int64_t r, int64_t w);
	void DrawRectangle(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w);
	void DrawRectangleFilled(int64_t x1, int64_t y1, int64_t x2, int64_t y2, int64_t w);
	void PlotPixel(int64_t x, int64_t y);
	void DrawText(int64_t x, int64_t y, std::string s);
	void DrawTextCentre(int64_t x, int64_t y, std::string s);
	void DrawTextRight(int64_t x, int64_t y, std::string s);
	void SetFont(std::string ff, std::string fs, int64_t size);
	void SetFG(uint32_t fg);
	void SetBG(uint32_t bg);
	void PrintString(const char* s);
	void PrintNewLine();
	void PrintTab(int64_t v);
private:
	// Clipping
	bool clip = false;
	int64_t clip_x1;
	int64_t clip_y1;
	int64_t clip_x2;
	int64_t clip_y2;
	int16_t left_id = 0;
	int16_t right_id = 0;
	int16_t top_id = 0;
	int16_t bottom_id = 0;
	lv_draw_mask_line_param_t line_mask_param_l;
	lv_draw_mask_line_param_t line_mask_param_r;
	lv_draw_mask_line_param_t line_mask_param_t;
	lv_draw_mask_line_param_t line_mask_param_b;

	OSDTask* task;
	int w;
	int h;
	int cursor_x = 0;
	int cursor_y = 0;
	int sz;
	lv_img_cf_t cf = LV_IMG_CF_TRUE_COLOR;
	lv_obj_t* firstbuffer = NULL;
	lv_obj_t* secondbuffer = NULL;
	bool which_buffer = false;
	bool double_buffered = false;
	lv_color_t fg;
	lv_color_t bg;
	uint8_t* buffer = nullptr;
	uint8_t* buffer_back = nullptr;
	lv_font_t* mono;
	lv_font_t* font;
	int size;
	int size_h;
	int size_v;

	void ScrollUp();
};
