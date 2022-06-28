#pragma once
#include <vector>
#include "../WindowManager/WindowManager.h"

enum class Mode {
	Insert,
	Overwrite
};

class Editor : public OSDTask {
public:
	Editor(int x, int y, int w, int h);

	void Run();
	void LoadSourceCode(std::string volume, std::string directory, std::string filename);
	void Render();
	lv_obj_t* GetObject() { return obj; }
	void RunWindowed();
	void FullscreenRun();
	void Debug();

	int screen_x = 0;
	int screen_y = 0;
	int x = 0;
	int y = 0;
	int canvas_w;
	int canvas_h;
private:
	Mode mode = Mode::Insert;
	size_t longest_line = 0;
	std::vector<std::string> code;
	lv_obj_t* obj;

	void CalculateLongestLine();
	static void ScrollEventHandler(lv_event_t* e);
	static void ResizeEventHandler(lv_event_t* e);
	static void ContextMenuEventHandler(lv_event_t* e);
	static void RunHandler(lv_event_t* e);
	static void RunFullScreenHandler(lv_event_t* e);
};
