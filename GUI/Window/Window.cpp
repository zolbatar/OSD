#include "Window.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include "LVGLWindow.h"

std::map<std::string, Window*> Window::windows;
extern int dm;

Window::Window(OSDTask* task, bool pure_canvas, bool fixed, std::string title, int x, int y, int w, int h)
		:task(task), title(title), x1(x), y1(y), width(w), height(h)
{
	x2 = x1+width;
	y2 = y1+height;

	// Create
	win = lv_mywin_create(lv_scr_act(), WINDOW_HEADER_HEIGHT*dm);
	lv_obj_set_pos(win, x1, y1);
	lv_obj_set_width(win, width);
	lv_obj_set_height(win, height);
	lv_obj_add_style(win, &style_window, LV_STATE_DEFAULT);

	// Header
	header = lv_mywin_get_header(win);
	lv_obj_add_style(header, &style_window_header, LV_STATE_DEFAULT);
	lv_obj_add_style(header, &style_window_header_inactive, LV_STATE_DEFAULT);
	lv_obj_add_event_cb(header, ClickEventHandler, LV_EVENT_CLICKED, this);
	lv_obj_add_event_cb(header, DragEventHandler, LV_EVENT_PRESSING, this);

	// Content
	auto content = lv_mywin_get_content(win);
	lv_obj_set_scrollbar_mode(content, LV_SCROLLBAR_MODE_AUTO);
	lv_obj_add_style(content, &style_scrollbar, LV_PART_SCROLLBAR);
	lv_obj_add_style(content, &style_window_content, LV_STATE_DEFAULT);

	// Title
	lv_mywin_add_title(win, title.c_str());

	auto btn_min = lv_mywin_add_btn(win, LV_SYMBOL_MINIMISE, WINDOW_FURNITURE_WIDTH*dm);
	lv_obj_add_style(btn_min, &style_window_furniture, LV_STATE_DEFAULT);
	auto btn_max = lv_mywin_add_btn(win, LV_SYMBOL_MAXIMISE, WINDOW_FURNITURE_WIDTH*dm);
	lv_obj_add_style(btn_max, &style_window_furniture, LV_STATE_DEFAULT);
	auto btn_close = lv_mywin_add_btn(win, LV_SYMBOL_MY_CLOSE, WINDOW_FURNITURE_WIDTH*dm);
	lv_obj_add_event_cb(btn_close, CloseClicked, LV_EVENT_CLICKED, this);
	lv_obj_add_style(btn_close, &style_window_furniture, LV_STATE_DEFAULT);

	if (pure_canvas) {
		canvas = new Canvas(task, content);
	}

	SetActive();
}

Window::~Window()
{
	if (canvas!=NULL)
		delete canvas;
	lv_obj_del(win);
}

void Window::SetActive()
{
	// Set all others as inactive
	for (auto& w: windows)
		w.second->SetInactive();

	lv_obj_add_style(header, &style_window_header_active, 0);
	lv_obj_move_foreground(this->GetLVGLWindow());
	this->active = true;
}

void Window::SetInactive()
{
	lv_obj_add_style(header, &style_window_header_inactive, 0);
	this->active = false;
}

void Window::CloseClicked(_lv_event_t* e)
{
	auto w = (Window*)e->user_data;
	w->task->RequestTerminate();
}

void Window::ClickEventHandler(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);
	auto win = (Window*)e->user_data;
	win->SetActive();
}

void Window::DragEventHandler(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);

	lv_indev_t* indev = lv_indev_get_act();
	if (indev==NULL) {
		return;
	}

	lv_point_t vect;
	lv_indev_get_vect(indev, &vect);

	lv_obj_t* win = lv_obj_get_parent(obj);

	// Set pos
	lv_coord_t x = lv_obj_get_x(win)+vect.x;
	lv_coord_t y = lv_obj_get_y(win)+vect.y;

	// Get size of window, will it fit?
	auto w = lv_obj_get_width(win);
	auto h = lv_obj_get_height(win);
	auto sw = lv_obj_get_width(lv_scr_act());
	auto sh = lv_obj_get_height(lv_scr_act());
	if (x<0)
		x = 0;
	if (x+w>sw)
		x = sw-w;
	if (y<0)
		y = 0;
	if (y+h>sh)
		y = sh-h;

	lv_obj_set_pos(win, x, y);
}
