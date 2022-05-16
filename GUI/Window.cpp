#include "Window.h"

Window::Window(bool pure_canvas, std::string title, int x, int y, int w, int h)
		:title(title), x1(x), y1(y), width(w), height(h)
{
	x2 = x1+width;
	y2 = y1+height;

	// Create
	win = lv_win_create(lv_scr_act(), WINDOW_HEADER_HEIGHT);
	lv_obj_set_pos(win, x1, y1);
	lv_obj_set_width(win, width+scrollbar_width+scrollbar_padding+scrollbar_padding);
	lv_obj_set_height(win, height+WINDOW_HEADER_HEIGHT+scrollbar_width+scrollbar_padding+scrollbar_padding);
	lv_obj_add_style(win, &style_window, 0);

	// Header
	header = lv_win_get_header(win);
	lv_obj_add_style(header, &style_window_header, 0);
	lv_obj_add_style(header, &style_window_header_inactive, 0);
	lv_obj_add_event_cb(header, DragEventHandler, LV_EVENT_PRESSING, NULL);

	// Content
	auto content = lv_win_get_content(win);
	lv_obj_set_scrollbar_mode(content, LV_SCROLLBAR_MODE_ON);
	lv_obj_add_style(content, &style_scrollbar, LV_PART_SCROLLBAR);
	lv_obj_add_style(content, &style_window_content, 0);

	// Title
	lv_win_add_title(win, title.c_str());

	// Window buttons
	auto btn_min = lv_win_add_btn(win, LV_SYMBOL_MINIMISE, WINDOW_FURNITURE_WIDTH);
	lv_obj_add_event_cb(btn_min, CloseClicked, LV_EVENT_CLICKED, NULL);
	lv_obj_add_style(btn_min, &style_window_furniture, 0);
	auto btn_max = lv_win_add_btn(win, LV_SYMBOL_MAXIMISE, WINDOW_FURNITURE_WIDTH);
	lv_obj_add_event_cb(btn_max, CloseClicked, LV_EVENT_CLICKED, NULL);
	lv_obj_add_style(btn_max, &style_window_furniture, 0);
	auto btn_close = lv_win_add_btn(win, LV_SYMBOL_CLOSE, WINDOW_FURNITURE_WIDTH);
	lv_obj_add_event_cb(btn_close, CloseClicked, LV_EVENT_CLICKED, NULL);
	lv_obj_add_style(btn_close, &style_window_furniture, 0);

	if (pure_canvas) {
		canvas = new Canvas(lv_win_get_content(win), w-(WINDOW_BORDER_WIDTH*2), h-(WINDOW_BORDER_WIDTH*2));
	}
}

Window::~Window()
{
	if (canvas!=NULL)
		delete canvas;
	lv_obj_del(win);
}

void Window::SetActive()
{
	lv_obj_add_style(header, &style_window_header_active, 0);
	this->active = true;
}

void Window::SetInactive()
{
	lv_obj_add_style(header, &style_window_header_inactive, 0);
	this->active = false;
}

void Window::CloseClicked(_lv_event_t* e)
{

}

void Window::DragEventHandler(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);

	lv_indev_t* indev = lv_indev_get_act();
	if (indev==NULL) return;

	lv_point_t vect;
	lv_indev_get_vect(indev, &vect);

	lv_obj_t* win = lv_obj_get_parent(obj);
	lv_coord_t x = lv_obj_get_x(win)+vect.x;
	lv_coord_t y = lv_obj_get_y(win)+vect.y;
	lv_obj_set_pos(win, x, y);
}
