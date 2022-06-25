#pragma once

const int MESSAGE_BLOCK = 512;

// These should really be dynamic, for later modules
enum class Messages {
	Canvas_Mode,
	Canvas_Enable_Shadow,
	Canvas_Circle,
	Canvas_CircleFilled,
	Canvas_ClipOn,
	Canvas_ClipOff,
	Canvas_Clear,
	Canvas_DrawLine,
	Canvas_Flip,
	Canvas_SetFont,
	Canvas_PlotPixel,
	Canvas_PrintString,
	Canvas_PrintNewLine,
	Canvas_PrintTab,
	Canvas_Rectangle,
	Canvas_RectangleFilled,
	Canvas_SetForegroundColour,
	Canvas_SetBackgroundColour,
	Canvas_Text,
	Canvas_TextCentre,
	Canvas_TextRight,
	Canvas_Triangle,
	Canvas_TriangleFilled,
	WM_CloseWindow,
	WM_OpenWindow,
};

struct WM_OpenWindow {
	char id[64];
	char title[256];
	int64_t x;
	int64_t y;
	int64_t width;
	int64_t height;
	bool canvas;
	bool fixed;
	int64_t canvas_w;
	int64_t canvas_h;
};

struct SetFont {
	int64_t ff;
	int64_t fs;
	int64_t size;
};

struct Coord1 {
	int64_t x;
	int64_t y;
};

struct Coord1S {
	int64_t x;
	int64_t y;
	int64_t s;
};

struct Coord1RW {
	int64_t x;
	int64_t y;
	int64_t r;
	int64_t w;
};

struct Coord2 {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
};

struct Coord2W {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
	int64_t w;
};

struct Coord3 {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
	int64_t x3;
	int64_t y3;
};

struct Coord3W {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
	int64_t x3;
	int64_t y3;
	int64_t w;
};

struct Colour {
	uint32_t colour;
};

struct Address {
	void* address;
};

struct Integer {
	int64_t v;
};
