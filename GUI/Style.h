#pragma once
#include "lvgl.h"

const int WINDOW_HEADER_HEIGHT = 28;
const int MENU_HEADER_HEIGHT = 32;
const int WINDOW_FURNITURE_WIDTH = 24;
const int WINDOW_BORDER_WIDTH = 1;
const int WINDOW_CONTENT_PADDING = 16;
const int CORNER_RADIUS = 3;
const int SCROLL_CORNER_RADIUS = 4;
const int CORNER_RADIUS_INNER = 3;
const int SCROLLBAR_WIDTH = 8;
const int CONTROL_PADDING = 1;
const int CONTAINER_PADDING = 8;

const lv_color_t MENU_BACKGROUND_COLOUR = lv_color_white();
const lv_color_t MENU_FOREGROUND_COLOUR = lv_color_black();
const lv_color_t DESKTOP_COLOUR = lv_color_hex(0x707070);
const lv_color_t HEADER_TEXT_COLOUR = lv_color_hex(0x000000);
const lv_color_t HEADER_BACKGROUND_COLOUR = lv_color_hex(0xB0B0B0);
const lv_color_t HEADER_BACKGROUND_COLOUR_ACTIVE = lv_color_hex(0xe0e0b0);
const lv_color_t WINDOW_BACKGROUND_COLOUR = lv_color_hex(0xD0D0D0);
const lv_color_t CONTAINER_BACKGROUND_COLOUR = WINDOW_BACKGROUND_COLOUR;
const lv_color_t CONTROL_BACKGROUND_COLOUR = lv_color_hex(0xE8E8E8);
const lv_color_t CONTROL_BORDER_COLOUR = lv_color_hex(0x404040);
const lv_color_t CONTROL_HIGHLIGHT_COLOUR = lv_color_hex(0xA0A0A0);
const lv_color_t WINDOW_FURNITURE_BACKGROUND_COLOUR = lv_color_hex(0xE0E0E0);
const lv_color_t WINDOW_FURNITURE_FOREGROUND_COLOUR = lv_color_hex(0x000000);
const lv_color_t WINDOW_FURNITURE_BORDER_COLOUR = lv_color_hex(0x404040);
const lv_color_t WINDOW_BORDER_COLOUR = lv_color_hex(0x404040);
const lv_color_t SCROLLBAR_BACKGROUND = lv_color_hex(0x404040);

extern lv_style_t style_background;
extern lv_style_t style_menu;
extern lv_style_t style_menu_container;
extern lv_style_t style_menu_item;
extern lv_style_t style_menu_item_bold;
extern lv_style_t style_window;
extern lv_style_t style_window_content;
extern lv_style_t style_iconbar;
extern lv_style_t style_iconbar_inner;
extern lv_style_t style_iconbar_button;
extern lv_style_t style_grid;
extern lv_style_t style_grid_title;
extern lv_style_t style_window_header;
extern lv_style_t style_window_header_inactive;
extern lv_style_t style_window_header_active;
extern lv_style_t style_window_furniture;
extern lv_style_t style_scrollbar;
extern lv_style_t style_bar;
extern lv_style_t style_bar_indicator;
extern lv_style_t style_bar_indicator_green;
extern lv_style_t style_bar_indicator_red;
extern lv_style_t style_chart;
extern lv_style_t style_chart_bar;
extern lv_style_t style_textarea;
extern lv_style_t style_boldbodyfont;
extern lv_style_t style_fontsymbol;
extern lv_font_t* font_mono;

#define LV_SYMBOL_MY_CLOSE        "\xEF\x8B\x93" // 0xF00D close
#define LV_SYMBOL_MAXIMISE        "\xEF\x8B\x90" // 0xF2D0 window maximize
#define LV_SYMBOL_MINIMISE        "\xEF\x8B\x91" // 0xF2D1 window minimise
#define LV_SYMBOL_RESIZE          "\xEF\x8C\x9D" // 0xf31D
#define LV_SYMBOL_MENURIGHT       "\xEF\x81\x94" // 0xf054
#define LV_SYMBOL_LAYOUT       	  "\xEE\x8A\x8F" // 0xf054
#define LV_SYMBOL_FILE       	  "\xEF\x85\x9B" // 0xf15b

//#define LV_SYMBOL_MY_CLOSE        "\xEE\xAE\x98" // 0xeb98 close
//#define LV_SYMBOL_MAXIMISE        "\xEF\x8B\x90" // 0xF2D0 window maximize
//#define LV_SYMBOL_MINIMISE        "\xEF\x8B\x91" // 0xF2D1 window minimise
//#define LV_SYMBOL_RESIZE          "\xEF\x8C\x9D" // 0xf31D

