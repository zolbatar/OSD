#pragma once
#include "lvgl.h"

const lv_color_t DESKTOP_COLOUR = lv_color_hex(0x707070);
const lv_color_t HEADER_TEXT_COLOUR = lv_color_hex(0x000000);
const lv_color_t HEADER_BACKGROUND_COLOUR = lv_color_hex(0xB0B0B0);
const lv_color_t HEADER_BACKGROUND_COLOUR_ACTIVE = lv_color_hex(0xe0e0b0);
const lv_color_t WINDOW_BACKGROUND_COLOUR = lv_color_hex(0xF0F0F0);
const lv_color_t CONTAINER_BACKGROUND_COLOUR = WINDOW_BACKGROUND_COLOUR;/*lv_color_hex(0xb0b0b0);*/
const lv_color_t CONTROL_BACKGROUND_COLOUR = lv_color_hex(0xE8E8E8);
const lv_color_t CONTROL_BORDER_COLOUR = lv_color_hex(0xA0A0A0);
const lv_color_t CONTROL_HIGHLIGHT_COLOUR = lv_color_hex(0x3973ac);
const lv_color_t WINDOW_FURNITURE_BACKGROUND_COLOUR = lv_color_hex(0xE0E0E0);
const lv_color_t WINDOW_FURNITURE_FOREGROUND_COLOUR = lv_color_hex(0x000000);
const lv_color_t WINDOW_FURNITURE_BORDER_COLOUR = lv_color_hex(0x404040);
const lv_color_t WINDOW_BORDER_COLOUR = lv_color_hex(0x000000);
const lv_color_t SCROLLBAR_BACKGROUND = lv_color_hex(0x8bd1de);
const int WINDOW_HEADER_HEIGHT = 32;
const int WINDOW_FURNITURE_WIDTH = 24;
const int WINDOW_BORDER_WIDTH = 1;
extern const lv_color_t DESKTOP_COLOUR;
extern const lv_color_t HEADER_TEXT_COLOUR;
extern const lv_color_t WINDOW_BORDER_COLOUR;
extern lv_style_t style_background;
extern lv_style_t style_window;
extern lv_style_t style_window_content;
extern lv_style_t style_grid;
extern lv_style_t style_window_header;
extern lv_style_t style_window_header_inactive;
extern lv_style_t style_window_header_active;
extern lv_style_t style_window_furniture;
extern lv_style_t style_scrollbar;
extern lv_style_t style_bar;
extern lv_style_t style_bar_indicator;
extern lv_style_t style_chart;
extern lv_style_t style_chart_bar;

#define LV_SYMBOL_CLOSE           "\xEF\x80\x8D" // 0xF00D close
#define LV_SYMBOL_MAXIMISE        "\xEF\x8B\x90" // 0xF2D0 window maximize
#define LV_SYMBOL_MINIMISE        "\xEF\x8B\x91" // 0xF2D1 window minimise
#define LV_SYMBOL_RESIZE          "\xEF\x81\xA6" // 0xf066
// 0xF00D,0xF2D0,0xF2D1,0xf066

void SetupLVGLStyles();

const int corner_radius = 4;
const int corner_radius_inner = 1;
const int scrollbar_width = 8;
const int scrollbar_padding = 3;
const int control_padding = 3;
