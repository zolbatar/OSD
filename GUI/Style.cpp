#include "Style.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include "../Tasks/FontManager/FontManager.h"

lv_style_t style_background;
lv_style_t style_window;
lv_style_t style_window_content;
lv_style_t style_grid;
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
size_t body_font_height = 18;

void SetupLVGLStyles()
{
	// Additional fonts
	auto font_window = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", 22);
	auto font_body = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", body_font_height);
	auto font_symbol = FontManager::GetFontByNameStyleAndSize("Symbols", "Light", 14);
	auto font_mono = FontManager::GetFontByNameStyleAndSize("IBM Plex Mono", "Regular", 22);

	// Disable scrolling
	lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

	// Style - background
	lv_style_init(&style_background);
	lv_style_set_bg_color(&style_background, DESKTOP_COLOUR);
	lv_style_set_text_color(&style_background, lv_color_white());
	lv_obj_add_style(lv_scr_act(), &style_background, LV_STATE_DEFAULT);

	// Style - window
	lv_style_init(&style_window);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_text_color(&style_window, lv_color_white());
	lv_style_set_border_width(&style_window, WINDOW_BORDER_WIDTH);
	lv_style_set_radius(&style_window, corner_radius);
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
	lv_style_set_border_width(&style_window_header, 1);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_border_side(&style_window_header, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_pad_all(&style_window_header, 4);

	// Style - window header inactive/active
	lv_style_init(&style_window_header_inactive);
	lv_style_set_bg_color(&style_window_header_inactive, HEADER_BACKGROUND_COLOUR);
	lv_style_init(&style_window_header_active);
	lv_style_set_bg_color(&style_window_header_active, HEADER_BACKGROUND_COLOUR_ACTIVE);

	// Style - window buttons
	lv_style_init(&style_window_furniture);
	lv_style_set_radius(&style_window_furniture, corner_radius_inner);
	lv_style_set_border_color(&style_window_furniture, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_width(&style_window_furniture, 1);
	lv_style_set_bg_color(&style_window_furniture, WINDOW_FURNITURE_BACKGROUND_COLOUR);
	lv_style_set_bg_opa(&style_window_furniture, LV_OPA_COVER);
	lv_style_set_text_color(&style_window_furniture, WINDOW_FURNITURE_FOREGROUND_COLOUR);
	lv_style_set_text_font(&style_window_furniture, font_symbol);

	// Style - scrollbar
	lv_style_init(&style_scrollbar);
//	lv_style_set_radius(&style_scrollbar, corner_radius);
	lv_style_set_border_color(&style_scrollbar, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_border_width(&style_scrollbar, 1);
//	lv_style_set_pad_all(&style_scrollbar, scrollbar_padding);
	lv_style_set_width(&style_scrollbar, scrollbar_width);
	lv_style_set_bg_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_bg_color(&style_scrollbar, SCROLLBAR_BACKGROUND);

	// Style - bar
	lv_style_init(&style_bar);
	lv_style_set_radius(&style_bar, corner_radius_inner);
	lv_style_set_border_color(&style_bar, CONTROL_BORDER_COLOUR);
	lv_style_set_border_width(&style_bar, 1);
	lv_style_set_pad_all(&style_bar, control_padding);
	lv_style_set_bg_color(&style_bar, CONTROL_BACKGROUND_COLOUR);
	lv_style_init(&style_bar_indicator);
	lv_style_set_radius(&style_bar_indicator, corner_radius_inner);
	lv_style_set_border_width(&style_bar_indicator, 0);
	lv_style_set_bg_color(&style_bar_indicator, CONTROL_HIGHLIGHT_COLOUR);
	lv_style_init(&style_bar_indicator_green);
	lv_style_set_radius(&style_bar_indicator_green, corner_radius_inner);
	lv_style_set_border_width(&style_bar_indicator_green, 0);
	lv_style_set_bg_color(&style_bar_indicator_green, lv_color_mix(CONTROL_HIGHLIGHT_COLOUR, lv_color_hex(0x00FF000), 192));
	lv_style_init(&style_bar_indicator_red);
	lv_style_set_radius(&style_bar_indicator_red, corner_radius_inner);
	lv_style_set_border_width(&style_bar_indicator_red, 0);
	lv_style_set_bg_color(&style_bar_indicator_red, lv_color_mix(CONTROL_HIGHLIGHT_COLOUR, lv_color_hex(0xFF00000), 192));

	// Style - grid
	lv_style_init(&style_grid);
	lv_style_set_bg_color(&style_grid, CONTAINER_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_grid, 0);
	lv_style_set_pad_all(&style_grid, 8);
	lv_style_set_radius(&style_grid, corner_radius_inner);

	// Style - chart
	lv_style_init(&style_chart);
	lv_style_set_bg_color(&style_chart, CONTROL_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_chart, 1);
	lv_style_set_border_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_line_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_pad_all(&style_chart, 4);
	lv_style_set_pad_column(&style_chart, 1);
	lv_style_set_radius(&style_chart, corner_radius_inner);
	lv_style_init(&style_chart_bar);
	lv_style_set_border_width(&style_chart_bar, 0);
	lv_style_set_pad_all(&style_chart_bar, 0);
	lv_style_set_radius(&style_chart_bar, corner_radius_inner);

	// Style - textarea
	lv_style_init(&style_textarea);
	lv_style_set_bg_color(&style_textarea, lv_color_white());
	lv_style_set_border_width(&style_textarea, 1);
	lv_style_set_pad_all(&style_textarea, 8);
	lv_style_set_radius(&style_textarea, corner_radius_inner);
	lv_style_set_text_font(&style_textarea, font_mono);
}