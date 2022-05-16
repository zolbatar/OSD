#pragma once
#include "lvgl.h"

const int WINDOW_HEADER_HEIGHT = 32;
const int WINDOW_FURNITURE_WIDTH = 24;
const int WINDOW_BORDER_WIDTH = 1;
extern const lv_color_t DESKTOP_COLOUR;
extern const lv_color_t HEADER_TEXT_COLOUR;
extern const lv_color_t HEADER_COLOUR;
extern const lv_color_t WINDOW_BORDER_COLOUR;
extern lv_style_t style_background;
extern lv_style_t style_window;
extern lv_style_t style_window_content;
extern lv_style_t style_window_header;
extern lv_style_t style_window_header_inactive;
extern lv_style_t style_window_header_active;
extern lv_style_t style_window_furniture;
extern lv_style_t style_scrollbar;
extern const lv_font_t* font_normal;
extern const lv_font_t* font_symbols;

#define LV_SYMBOL_SENDTOBACK      "\xEF\xA1\xBF" // 0xF87F send-backward
#define LV_SYMBOL_CLOSE           "\xEF\x80\x8D" // 0xF00D close
#define LV_SYMBOL_MAXIMISE        "\xEF\x8B\x90" // 0xF2D0 window maximize
#define LV_SYMBOL_MINIMISE        "\xEF\x8B\x91" // 0xF2D1 window minimise
#define LV_SYMBOL_RESIZE          "\xEF\x81\xA5" // 0xF065 expand
// 0xF00D, 0xF87F,0xF2D0,0xF2D1,0xF065

void SetupLVGLStyles();