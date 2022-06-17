#include "Style.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include "../Tasks/FontManager/FontManager.h"
#include "../Tasks/WindowManager/WindowManager.h"

extern int ScreenResX;
extern int ScreenResY;
lv_style_t style_background;
lv_style_t style_menu;
lv_style_t style_menu_container;
lv_style_t style_menu_item;
lv_style_t style_window;
lv_style_t style_window_content;
lv_style_t style_grid;
lv_style_t style_grid_title;
lv_style_t style_window_header;
lv_style_t style_window_header_active;
lv_style_t style_window_header_inactive;
lv_style_t style_window_furniture;
lv_style_t style_scrollbar;
lv_style_t style_bar;
lv_style_t style_bar_indicator;
lv_style_t style_bar_indicator_green;
lv_style_t style_bar_indicator_red;
lv_style_t style_chart_bar;
lv_style_t style_chart;
lv_style_t style_textarea;
lv_style_t style_boldbodyfont;
size_t body_font_height = 20;
size_t menu_font_height = 24;
lv_font_t* font_mono;
lv_font_t* font_body;
int dm = 1;

void WindowManager::SetupLVGLStyles()
{
	CLogger::Get()->Write("Window Manager", LogDebug, "Setting up styles");
/*	auto font_window = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", menu_font_height*dm);
	font_body = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", body_font_height*dm);
	auto font_body_bold = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Bold", body_font_height*dm);
	auto menu_body = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", menu_font_height*dm);
	font_mono = FontManager::GetFontByNameStyleAndSize("Source Code Pro", "Regular", body_font_height*dm);*/

	auto font_window = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", menu_font_height*dm);
	font_body = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", body_font_height*dm);
	auto font_body_bold = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Bold", body_font_height*dm);
	auto font_large = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Bold", (body_font_height+4)*dm);
	auto menu_body = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", menu_font_height*dm);
	font_mono = FontManager::GetFontByNameStyleAndSize("IBM Plex Mono", "Regular", body_font_height*dm);

	auto font_symbol = FontManager::GetFontByNameStyleAndSize("Font Awesome 6 Pro Light", "Light", 16*dm);

	lv_style_init(&style_boldbodyfont);
	lv_style_set_text_font(&style_boldbodyfont, font_body_bold);

	// Disable scrolling
	lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

	// Style - background
	fs.SetCurrentDirectory(":BOOT.$.System.Wallpaper");
	FIL fil;
	if (f_open(&fil, (fs.GetCurrentDirectory()+"Wallpaper.bin").c_str(), FA_READ | FA_OPEN_EXISTING)!=FR_OK) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error opening wallpaper file");
	}
	size_t sz = f_size(&fil);
	char* buffer = (char*)malloc(sz);
	if (!buffer) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error allocating memory for wallpaper file");
	}
	uint32_t l;
	if (f_read(&fil, buffer, sz, &l)!=FR_OK) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error loading wallpaper file");
	}
	static lv_img_dsc_t img;
	img.data = (const uint8_t*)buffer;
	img.header.w = ScreenResX;
	img.header.h = ScreenResY;
	img.header.cf = LV_IMG_CF_TRUE_COLOR;
	img.data_size = l;

	lv_style_init(&style_background);
	lv_style_set_bg_img_src(&style_background, &img);
//	lv_style_set_bg_color(&style_background, DESKTOP_COLOUR);
	lv_style_set_text_color(&style_background, lv_color_white());
	lv_obj_add_style(lv_scr_act(), &style_background, LV_STATE_DEFAULT);

	// Style - menu
	lv_style_init(&style_menu);
	lv_style_set_border_color(&style_menu, WINDOW_BORDER_COLOUR);
	lv_style_set_text_color(&style_menu, lv_color_white());
	lv_style_set_border_width(&style_menu, WINDOW_BORDER_WIDTH*dm);
	lv_style_set_radius(&style_menu, corner_radius_inner*dm);
	lv_style_set_text_font(&style_menu, menu_body);
	lv_style_init(&style_menu_container);
	lv_style_set_bg_color(&style_menu_container, CONTAINER_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_menu_container, 0);
	lv_style_set_pad_all(&style_menu_container, 2*dm);
	lv_style_set_radius(&style_menu_container, corner_radius_inner*dm);
	lv_style_set_text_font(&style_menu_container, menu_body);
	lv_style_init(&style_menu_item);
	lv_style_set_pad_all(&style_menu_item, 0);
	lv_style_set_text_font(&style_menu_item, menu_body);

	// Style - window
	lv_style_init(&style_window);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_text_color(&style_window, lv_color_white());
	lv_style_set_border_width(&style_window, WINDOW_BORDER_WIDTH*dm);
	lv_style_set_radius(&style_window, corner_radius*dm);
	lv_style_set_text_font(&style_window, font_window);

	// Style - window content
	lv_style_init(&style_window_content);
	lv_style_set_bg_color(&style_window_content, WINDOW_BACKGROUND_COLOUR);
	lv_style_set_pad_all(&style_window_content, 0);
	lv_style_set_text_font(&style_window_content, font_body);
	lv_style_set_text_color(&style_window_content, lv_color_black());

	// Style - window header
	lv_style_init(&style_window_header);
	lv_style_set_text_color(&style_window_header, HEADER_TEXT_COLOUR);
	lv_style_set_border_width(&style_window_header, 1*dm);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_border_side(&style_window_header, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_pad_all(&style_window_header, 4*dm);

	// Style - window header inactive/active
	lv_style_init(&style_window_header_inactive);
	lv_style_set_bg_color(&style_window_header_inactive, HEADER_BACKGROUND_COLOUR);
	lv_style_init(&style_window_header_active);
	lv_style_set_bg_color(&style_window_header_active, HEADER_BACKGROUND_COLOUR_ACTIVE);

	// Style - window buttons
	lv_style_init(&style_window_furniture);
	lv_style_set_radius(&style_window_furniture, corner_radius_inner*dm);
	lv_style_set_border_color(&style_window_furniture, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_width(&style_window_furniture, 1*dm);
	lv_style_set_bg_color(&style_window_furniture, WINDOW_FURNITURE_BACKGROUND_COLOUR);
	lv_style_set_bg_opa(&style_window_furniture, LV_OPA_COVER);
	lv_style_set_text_color(&style_window_furniture, WINDOW_FURNITURE_FOREGROUND_COLOUR);
	lv_style_set_pad_all(&style_window_furniture, 0);
	lv_style_set_text_font(&style_window_furniture, font_symbol);

	// Style - scrollbar
	lv_style_init(&style_scrollbar);
//	lv_style_set_radius(&style_scrollbar, corner_radius);
	lv_style_set_border_color(&style_scrollbar, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_border_width(&style_scrollbar, 1*dm);
//	lv_style_set_pad_all(&style_scrollbar, scrollbar_padding);
	lv_style_set_width(&style_scrollbar, scrollbar_width*dm);
	lv_style_set_bg_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_bg_color(&style_scrollbar, SCROLLBAR_BACKGROUND);

	// Style - bar
	lv_style_init(&style_bar);
	lv_style_set_radius(&style_bar, 0);
	lv_style_set_border_color(&style_bar, CONTROL_BORDER_COLOUR);
	lv_style_set_border_width(&style_bar, 1*dm);
	lv_style_set_pad_all(&style_bar, control_padding*dm);
	lv_style_set_bg_color(&style_bar, CONTROL_BACKGROUND_COLOUR);
	lv_style_init(&style_bar_indicator);
	lv_style_set_radius(&style_bar_indicator, 0);
	lv_style_set_border_width(&style_bar_indicator, 0);
	lv_style_set_bg_color(&style_bar_indicator, CONTROL_HIGHLIGHT_COLOUR);
	lv_style_init(&style_bar_indicator_green);
	lv_style_set_radius(&style_bar_indicator_green, 0);
	lv_style_set_border_width(&style_bar_indicator_green, 0);
	lv_style_set_bg_color(&style_bar_indicator_green, lv_color_mix(CONTROL_HIGHLIGHT_COLOUR, lv_color_hex(0x00FF000), 192));
	lv_style_init(&style_bar_indicator_red);
	lv_style_set_radius(&style_bar_indicator_red, 0);
	lv_style_set_border_width(&style_bar_indicator_red, 0);
	lv_style_set_bg_color(&style_bar_indicator_red, lv_color_mix(CONTROL_HIGHLIGHT_COLOUR, lv_color_hex(0xFF00000), 192));

	// Style - grid
	lv_style_init(&style_grid);
	lv_style_set_bg_color(&style_grid, CONTAINER_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_grid, 0);
	lv_style_set_pad_all(&style_grid, 8*dm);
	lv_style_set_radius(&style_grid, corner_radius_inner);
	lv_style_init(&style_grid_title);
	lv_style_set_text_font(&style_grid_title, font_large);

	// Style - chart
	lv_style_init(&style_chart);
	lv_style_set_bg_color(&style_chart, CONTROL_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_chart, 1*dm);
	lv_style_set_border_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_line_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_pad_all(&style_chart, 4*dm);
	lv_style_set_pad_column(&style_chart, 1*dm);
	lv_style_set_radius(&style_chart, corner_radius_inner*dm);
	lv_style_init(&style_chart_bar);
	lv_style_set_border_width(&style_chart_bar, 0);
	lv_style_set_pad_all(&style_chart_bar, 0);
	lv_style_set_radius(&style_chart_bar, corner_radius_inner);

	// Style - textarea
	lv_style_init(&style_textarea);
	lv_style_set_bg_color(&style_textarea, lv_color_white());
	lv_style_set_border_width(&style_textarea, 1*dm);
	lv_style_set_pad_all(&style_textarea, 8*dm);
	lv_style_set_radius(&style_textarea, corner_radius_inner);
	lv_style_set_text_font(&style_textarea, font_mono);
}