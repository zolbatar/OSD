#include "Window.h"
#ifndef CLION
#include <circle/logger.h>
#endif
#include "LVGLWindow.h"

std::map<std::string, Window*> Window::windows;

Window::Window(OSDTask* task, bool pure_canvas, bool fixed, std::string title, int x, int y, int w, int h, int canvas_w, int canvas_h)
		:task(task), title(title), x1(x), y1(y), width(w), height(h)
{
	x2 = x1+width;
	y2 = y1+height;

	// Set attributes
	WindowAttributes* wa = new WindowAttributes();
	wa->resizable = true;
	wa->fixed_size_content = pure_canvas;

	// Create
	win = lv_mywin_create(lv_scr_act(), ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight), wa);
	lv_obj_set_pos(win, x1, y1);
	lv_obj_set_width(win, width);
	lv_obj_set_height(win, height);

	// Content
	auto content = lv_mywin_get_content(win);

	// Header
	header = lv_mywin_get_header(win);
	lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), LV_STATE_DEFAULT);
	lv_obj_add_event_cb(header, ClickEventHandler, LV_EVENT_CLICKED, this);
	lv_obj_add_event_cb(header, DragEventHandler, LV_EVENT_PRESSING, this);

	// Title
	lv_mywin_add_title(win, title.c_str());

	auto furniture_width = ThemeManager::GetConst(ConstAttribute::WindowFurnitureWidth);
	auto style = ThemeManager::GetStyle(StyleAttribute::WindowButton);
	auto btn_min = lv_mywin_add_btn(win, LV_SYMBOL_MINIMISE, furniture_width);
	lv_obj_add_style(btn_min, style, LV_STATE_DEFAULT);
	auto btn_max = lv_mywin_add_btn(win, LV_SYMBOL_MAXIMISE, furniture_width);
	lv_obj_add_style(btn_max, style, LV_STATE_DEFAULT);
	auto btn_close = lv_mywin_add_btn(win, LV_SYMBOL_MY_CLOSE, furniture_width);
	lv_obj_add_event_cb(btn_close, CloseClicked, LV_EVENT_CLICKED, this);
	lv_obj_add_style(btn_close, style, LV_STATE_DEFAULT);

	if (pure_canvas) {
		canvas = new Canvas(task, content, canvas_w, canvas_h);
	}

	group = lv_group_create();
	lv_group_add_obj(group, win);
	SetActive();
}

Window::~Window()
{
	if (canvas!=NULL)
		delete canvas;
	lv_group_del(group);
	lv_obj_del(win);
}

void Window::SetActive()
{
	// Set all others as inactive
	for (auto& w: windows)
		w.second->SetInactive();

	lv_obj_remove_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), 0);
	lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowActive), 0);
	lv_obj_move_foreground(this->GetLVGLWindow());
	this->active = true;

	// Focus
	GuiCLVGL::SetKeyboardGroup(group);
	lv_group_focus_obj(win);
}

void Window::SetInactive()
{
	lv_obj_remove_style(header, ThemeManager::GetStyle(StyleAttribute::WindowActive), 0);
	lv_obj_add_style(header, ThemeManager::GetStyle(StyleAttribute::WindowInactive), 0);
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
