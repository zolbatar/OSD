#pragma once
#include <string>
#include "../Controls/Control.h"
#include "../Controls/Canvas.h"

class Window {
public:
	Window(OSDTask* task, bool pure_canvas, bool fixed, std::string title, int x, int y, int w, int h, int canvas_w, int canvas_h);
	~Window();
	lv_obj_t* GetLVGLWindow() { return win; }
	Canvas* GetCanvas() { return canvas; }
	int64_t GetContentWidth() { return canvas->GetContentWidth(); }
	int64_t GetContentHeight() { return canvas->GetContentHeight(); }
	void SetActive();
	void SetInactive();
	bool GetActive() { return active; }

	static std::map<std::string, Window*> windows;
private:
	static void CloseClicked(_lv_event_t* e);
	static void DragEventHandler(lv_event_t* e);
	static void ClickEventHandler(lv_event_t* e);
	std::string title;
	int x1;
	int y1;
	int x2;
	int y2;
	int width;
	int height;
	int z = 0;
	bool active = false;
	lv_obj_t* win;
	lv_obj_t* header;
	Canvas* canvas = NULL;
	OSDTask* task;
	std::list<Control> children;
};

