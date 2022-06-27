#pragma once
#include <vector>
#include "../WindowManager/WindowManager.h"

class Editor : public OSDTask {
public:
	Editor(int x, int y, int w, int h);

	void Run();
	void LoadSourceCode(std::string volume, std::string directory, std::string filename);
	void Render();

	size_t x = 0;
	size_t y = 0;
private:
	int canvas_w;
	int canvas_h;
	size_t longest_line = 0;
	std::vector<std::string> code;
	lv_obj_t *obj;

	void CalculateLongestLine();
	static void ScrollEventHandler(lv_event_t* e);
};
