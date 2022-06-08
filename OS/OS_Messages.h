#pragma once

const int MESSAGE_BLOCK = 512;

// These should really be dynamic, for later modules
enum class Messages {
	Canvas_Clear,
	Canvas_DrawLine,
	Canvas_PlotPixel,
	Canvas_Triangle,
	Canvas_TriangleFilled,
	Canvas_SetForegroundColour,
	Canvas_SetBackgroundColour,
	Canvas_PrintString,
	Canvas_PrintStringLong,
	Canvas_PrintNewLine,
	Canvas_PrintTab,
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
};

struct Coord1 {
	int64_t x;
	int64_t y;
};

struct Coord2 {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
};

struct Coord3 {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
	int64_t x3;
	int64_t y3;
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
