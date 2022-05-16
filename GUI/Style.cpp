#include "Style.h"

const lv_color_t DESKTOP_COLOUR = lv_color_hex(0x707070);
const lv_color_t HEADER_TEXT_COLOUR = lv_color_hex(0x000000);
const lv_color_t HEADER_BACKGROUND_COLOUR = lv_color_hex(0xB0B0B0);
const lv_color_t HEADER_BACKGROUND_COLOUR_ACTIVE = lv_color_hex(0xe0e0b0);
const lv_color_t WINDOW_BACKGROUND_COLOUR = lv_color_hex(0xA0A0A0);
const lv_color_t WINDOW_FURNITURE_BACKGROUND_COLOUR = lv_color_hex(0xE0E0E0);
const lv_color_t WINDOW_FURNITURE_FOREGROUND_COLOUR = lv_color_hex(0x000000);
const lv_color_t WINDOW_FURNITURE_BORDER_COLOUR = lv_color_hex(0x404040);
const lv_color_t WINDOW_BORDER_COLOUR = lv_color_hex(0x000000);
const lv_color_t SCROLLBAR_BACKGROUND = lv_color_hex(0xD0D0D0);
lv_style_t style_background;
lv_style_t style_window;
lv_style_t style_window_content;
lv_style_t style_window_header;
lv_style_t style_window_header_active;
lv_style_t style_window_header_inactive;
lv_style_t style_window_furniture;
lv_style_t style_scrollbar;
const lv_font_t* font_normal;
const lv_font_t* font_symbols;

void SetupLVGLStyles()
{
	// Additional fonts
	LV_FONT_DECLARE(plex_sans_10);
	LV_FONT_DECLARE(plex_sans_12);
	LV_FONT_DECLARE(plex_sans_14);
	LV_FONT_DECLARE(plex_sans_16);
	LV_FONT_DECLARE(plex_sans_18);
	LV_FONT_DECLARE(plex_sans_20);
	LV_FONT_DECLARE(symbols_12);
	auto font_normal = &plex_sans_18;

	// Disable scrolling
	lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

	// Style - background
	lv_style_init(&style_background);
	lv_style_set_bg_color(&style_background, DESKTOP_COLOUR);
	lv_style_set_text_color(&style_background, lv_color_white());
	lv_style_set_text_font(&style_background, font_normal);
	lv_obj_add_style(lv_scr_act(), &style_background, LV_STATE_DEFAULT);

	// Style - window
	lv_style_init(&style_window);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_text_color(&style_window, lv_color_white());
	lv_style_set_border_width(&style_window, WINDOW_BORDER_WIDTH);
	lv_style_set_radius(&style_window, corner_radius);
	lv_style_set_text_font(&style_window, font_normal);

	// Style - window content
	lv_style_init(&style_window_content);
	lv_style_set_bg_color(&style_window_content, WINDOW_BACKGROUND_COLOUR);
	lv_style_set_pad_all(&style_window_content, 0);
	lv_style_set_text_font(&style_window_content, font_normal);

	// Style - window header
	lv_style_init(&style_window_header);
	lv_style_set_text_color(&style_window_header, HEADER_TEXT_COLOUR);
	lv_style_set_border_width(&style_window_header, 1);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_border_side(&style_window_header, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_text_font(&style_window_header, font_normal);
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
	lv_style_set_text_font(&style_window_furniture, &symbols_12);

	// Style - scrollbar
	lv_style_init(&style_scrollbar);
	lv_style_set_radius(&style_scrollbar, corner_radius_inner);
	lv_style_set_border_color(&style_scrollbar, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_width(&style_scrollbar, 1);
	lv_style_set_pad_all(&style_scrollbar, scrollbar_padding);
	lv_style_set_width(&style_scrollbar, scrollbar_width);
	lv_style_set_bg_opa(&style_scrollbar, LV_OPA_COVER);
	lv_style_set_bg_color(&style_scrollbar, SCROLLBAR_BACKGROUND);
}