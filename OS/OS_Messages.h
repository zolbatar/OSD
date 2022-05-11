#pragma once

// These should really be dynamic, for later modules
enum class Messages {
	Canvas_Clear,
	Canvas_DrawLine,
	Canvas_PlotPixel,
	Canvas_SetForegroundColour,
	WM_CloseWindow,
	WM_OpenWindow,
};

struct WM_OpenWindow {
	std::string id;
	std::string title;
	int64_t x;
	int64_t y;
	int64_t width;
	int64_t height;
};

struct Canvas_PlotPixel {
	int64_t x;
	int64_t y;
};

struct Canvas_DrawLine {
	int64_t x1;
	int64_t y1;
	int64_t x2;
	int64_t y2;
};

struct Canvas_Colour {
	uint32_t colour;
};
