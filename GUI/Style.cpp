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
lv_style_t style_iconbar;
lv_style_t style_iconbar_inner;
lv_style_t style_iconbar_button;
lv_style_t style_grid;
lv_style_t style_grid_title;
lv_style_t style_window_header;
lv_style_t style_window_header_active;
lv_style_t style_window_header_inactive;
lv_style_t style_window_furniture;
lv_style_t style_window_furniture_small;
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

void WindowManager::SetupLVGLStyles()
{
	CLogger::Get()->Write("Window Manager", LogDebug, "Setting up styles");
/*	auto font_window = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", menu_font_height);
	font_body = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", body_font_height);
	auto font_body_bold = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Bold", body_font_height);
	auto menu_body = FontManager::GetFontByNameStyleAndSize("Source Sans Pro", "Regular", menu_font_height);
	font_mono = FontManager::GetFontByNameStyleAndSize("Source Code Pro", "Regular", body_font_height);*/

	auto font_window = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", menu_font_height);
	font_body = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", body_font_height);
	auto font_body_bold = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Bold", body_font_height);
	auto font_small = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Bold", (body_font_height-2));
	auto font_large = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Bold", (body_font_height+4));
	auto menu_body = FontManager::GetFontByNameStyleAndSize("IBM Plex Sans", "Regular", menu_font_height);
	font_mono = FontManager::GetFontByNameStyleAndSize("IBM Plex Mono", "Regular", body_font_height);

	auto font_symbol = FontManager::GetFontByNameStyleAndSize("Font Awesome 6 Pro Light", "Light", 16);
	auto font_symbol_small = FontManager::GetFontByNameStyleAndSize("Font Awesome 6 Pro Light", "Light", 12);

	lv_style_init(&style_boldbodyfont);
	lv_style_set_text_font(&style_boldbodyfont, font_body_bold);

	// Disable scrolling
	lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

	// Style - menu
	lv_style_init(&style_menu);
	lv_style_set_border_color(&style_menu, WINDOW_BORDER_COLOUR);
	lv_style_set_text_color(&style_menu, lv_color_white());
	lv_style_set_border_width(&style_menu, WINDOW_BORDER_WIDTH);
	lv_style_set_radius(&style_menu, CORNER_RADIUS_INNER);
	lv_style_set_text_font(&style_menu, menu_body);
	lv_style_init(&style_menu_container);
	lv_style_set_bg_color(&style_menu_container, CONTAINER_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_menu_container, 0);
	lv_style_set_pad_all(&style_menu_container, 2);
	lv_style_set_radius(&style_menu_container, CORNER_RADIUS_INNER);
	lv_style_set_text_font(&style_menu_container, menu_body);
	lv_style_init(&style_menu_item);
	lv_style_set_pad_all(&style_menu_item, 0);
	lv_style_set_text_font(&style_menu_item, menu_body);

	// Style - icon bar
	lv_style_init(&style_iconbar);
	lv_style_set_border_color(&style_iconbar, WINDOW_BORDER_COLOUR);
	lv_style_set_border_side(&style_iconbar, LV_BORDER_SIDE_TOP);
	lv_style_set_border_width(&style_iconbar, WINDOW_BORDER_WIDTH);
	lv_style_set_bg_color(&style_iconbar, WINDOW_BACKGROUND_COLOUR);
	lv_style_set_radius(&style_iconbar, 0);
	lv_style_set_pad_all(&style_iconbar, 0);
	lv_style_set_pad_left(&style_iconbar, 8);
	lv_style_set_pad_right(&style_iconbar, 8);
	lv_style_init(&style_iconbar_inner);
	lv_style_set_text_color(&style_iconbar_inner, lv_color_black());
	lv_style_set_bg_color(&style_iconbar_inner, WINDOW_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_iconbar_inner, 0);
	lv_style_set_radius(&style_iconbar_inner, 0);
	lv_style_set_pad_all(&style_iconbar_inner, 0);
	lv_style_set_text_font(&style_iconbar_inner, font_body);

	// Style - window
	lv_style_init(&style_window);
	lv_style_set_border_color(&style_window, WINDOW_BORDER_COLOUR);
	lv_style_set_border_width(&style_window, WINDOW_BORDER_WIDTH);
	lv_style_set_radius(&style_window, CORNER_RADIUS);
	lv_style_set_text_font(&style_window, font_window);
	lv_style_set_clip_corner(&style_window, true);
	lv_style_set_border_post(&style_window, true);

	// Style - window content
	lv_style_init(&style_window_content);
	lv_style_set_border_width(&style_window_content, 0);
	lv_style_set_radius(&style_window_content, 0);
	lv_style_set_bg_color(&style_window_content, WINDOW_BACKGROUND_COLOUR);
	lv_style_set_bg_opa(&style_window_content, LV_OPA_COVER);
	lv_style_set_pad_all(&style_window_content, WINDOW_CONTENT_PADDING);
	lv_style_set_text_font(&style_window_content, font_body);
	lv_style_set_text_color(&style_window_content, lv_color_black());

	// Style - window header
	lv_style_init(&style_window_header);
	lv_style_set_radius(&style_window_header, 0);
	lv_style_set_text_color(&style_window_header, HEADER_TEXT_COLOUR);
	lv_style_set_border_width(&style_window_header, 1);
	lv_style_set_border_color(&style_window_header, WINDOW_BORDER_COLOUR);
	lv_style_set_border_side(&style_window_header, LV_BORDER_SIDE_BOTTOM);
	lv_style_set_pad_all(&style_window_header, 4);

	// Style - window header inactive/active
	lv_style_init(&style_window_header_inactive);
	lv_style_set_bg_color(&style_window_header_inactive, HEADER_BACKGROUND_COLOUR);
	lv_style_init(&style_window_header_active);
	lv_style_set_bg_color(&style_window_header_active, HEADER_BACKGROUND_COLOUR_ACTIVE);

	// Style - window buttons
	lv_style_init(&style_window_furniture);
	lv_style_set_radius(&style_window_furniture, CORNER_RADIUS_INNER);
	lv_style_set_border_color(&style_window_furniture, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_width(&style_window_furniture, 1);
	lv_style_set_bg_color(&style_window_furniture, WINDOW_FURNITURE_BACKGROUND_COLOUR);
	lv_style_set_bg_opa(&style_window_furniture, LV_OPA_COVER);
	lv_style_set_text_color(&style_window_furniture, WINDOW_FURNITURE_FOREGROUND_COLOUR);
	lv_style_set_pad_all(&style_window_furniture, 0);
	lv_style_set_text_font(&style_window_furniture, font_symbol);

	// Style - window buttons small
	lv_style_init(&style_window_furniture_small);
	lv_style_set_radius(&style_window_furniture_small, CORNER_RADIUS_INNER);
	lv_style_set_border_color(&style_window_furniture_small, WINDOW_FURNITURE_BORDER_COLOUR);
	lv_style_set_border_width(&style_window_furniture_small, 1);
	lv_style_set_bg_color(&style_window_furniture_small, WINDOW_FURNITURE_BACKGROUND_COLOUR);
	lv_style_set_bg_opa(&style_window_furniture_small, LV_OPA_COVER);
	lv_style_set_text_color(&style_window_furniture_small, WINDOW_FURNITURE_FOREGROUND_COLOUR);
	lv_style_set_pad_all(&style_window_furniture_small, 0);
	lv_style_set_text_font(&style_window_furniture_small, font_symbol_small);

	// Style - scrollbar
	lv_style_init(&style_scrollbar);
	lv_style_set_radius(&style_scrollbar, SCROLL_CORNER_RADIUS);
	lv_style_set_border_color(&style_scrollbar, CONTROL_BORDER_COLOUR);
	lv_style_set_border_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_border_width(&style_scrollbar, 1);
	lv_style_set_width(&style_scrollbar, SCROLLBAR_WIDTH);
	lv_style_set_height(&style_scrollbar, SCROLLBAR_WIDTH);
	lv_style_set_bg_opa(&style_scrollbar, LV_OPA_30);
	lv_style_set_bg_color(&style_scrollbar, SCROLLBAR_BACKGROUND);

	// Style - bar
	lv_style_init(&style_bar);
	lv_style_set_radius(&style_bar, 0);
	lv_style_set_border_color(&style_bar, CONTROL_BORDER_COLOUR);
	lv_style_set_border_width(&style_bar, 1);
	lv_style_set_pad_all(&style_bar, CONTROL_PADDING);
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
//	lv_style_set_border_color(&style_grid, CONTROL_BORDER_COLOUR);
	lv_style_set_border_width(&style_grid, 0);
	lv_style_set_pad_all(&style_grid, CONTAINER_PADDING);
	lv_style_set_radius(&style_grid, CORNER_RADIUS_INNER);
	lv_style_init(&style_grid_title);
	lv_style_set_text_font(&style_grid_title, font_large);

	// Style - chart
	lv_style_init(&style_chart);
	lv_style_set_bg_color(&style_chart, CONTROL_BACKGROUND_COLOUR);
	lv_style_set_border_width(&style_chart, 1);
	lv_style_set_border_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_line_color(&style_chart, CONTROL_BORDER_COLOUR);
	lv_style_set_pad_all(&style_chart, 4);
	lv_style_set_pad_column(&style_chart, 1);
	lv_style_set_radius(&style_chart, CORNER_RADIUS_INNER);
	lv_style_init(&style_chart_bar);
	lv_style_set_border_width(&style_chart_bar, 0);
	lv_style_set_pad_all(&style_chart_bar, 0);
	lv_style_set_radius(&style_chart_bar, CORNER_RADIUS_INNER);

	// Style - textarea
	lv_style_init(&style_textarea);
	lv_style_set_bg_color(&style_textarea, lv_color_white());
	lv_style_set_border_width(&style_textarea, 1);
	lv_style_set_pad_all(&style_textarea, 8);
	lv_style_set_radius(&style_textarea, CORNER_RADIUS_INNER);
	lv_style_set_text_font(&style_textarea, font_mono);

	// Style - icon bar buttons
	lv_style_init(&style_iconbar_button);
	lv_style_set_radius(&style_iconbar_button, 0);
	lv_style_set_shadow_width(&style_iconbar_button, 0);
	lv_style_set_border_width(&style_iconbar_button, 0);
	lv_style_set_bg_opa(&style_iconbar_button, LV_OPA_TRANSP);
	lv_style_set_pad_all(&style_iconbar_button, 0);
	lv_style_set_text_font(&style_iconbar_button, font_large);

	// Wallpaper
	fs.SetCurrentDirectory(":BOOT.$.System.Wallpaper");
	auto img = LoadPNG("Wallpaper.png", ScreenResX, ScreenResY);
	lv_style_init(&style_background);
	lv_style_set_bg_img_src(&style_background, img);
	//	lv_style_set_bg_color(&style_background, DESKTOP_COLOUR);
	lv_style_set_text_color(&style_background, lv_color_black());
	lv_obj_add_style(lv_scr_act(), &style_background, LV_STATE_DEFAULT);

	// Icons
	fs.SetCurrentDirectory(":BOOT.$.System.Icons");
	LoadIcon("Flash.png", "SD Card");
	LoadIcon("RAM2.png", "RAM");
	LoadIcon("Boot.png", "Boot");
	LoadIcon("Home.png", "Home");
	LoadIcon("Sloth.png", "Sloth");
	LoadIcon("Folder.png", "Folder");

	// Cursors
	fs.SetCurrentDirectory(":BOOT.$.System.Cursors");
	mouse_cursor = LoadPNG("Arrow.png", 32, 32);
}

void WindowManager::LoadIcon(std::string filename, std::string name)
{
	auto img = LoadPNG(filename, 64, 64);
	icons.insert(std::make_pair(name, img));
}

lv_img_dsc_t* WindowManager::LoadPNG(std::string filename, int w, int h)
{
	FIL fil;
	if (f_open(&fil, (fs.GetCurrentDirectory()+filename).c_str(), FA_READ | FA_OPEN_EXISTING)!=FR_OK) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error opening image file '%s'", filename.c_str());
	}
	size_t sz = f_size(&fil);
	char* buffer = (char*)malloc(sz);
	if (!buffer) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error allocating memory for image file '%s'", filename.c_str());
	}
	uint32_t l;
	if (f_read(&fil, buffer, sz, &l)!=FR_OK) {
		CLogger::Get()->Write("Window Manager", LogPanic, "Error loading image file '%s'", filename.c_str());
	}
	f_close(&fil);

	// Convert to PNG
	auto img = (lv_img_dsc_t*)calloc(1, sizeof(lv_img_dsc_t));
	img->data = (const uint8_t*)buffer;
	img->header.w = w;
	img->header.h = h;
	img->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
	img->data_size = l;
	return img;
}