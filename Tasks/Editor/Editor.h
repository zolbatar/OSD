#pragma once
#include "../WindowManager/WindowManager.h"

class Editor : public OSDTask {
public:
	Editor(int x, int y, int w, int h);

	void Run();
	void LoadSourceCode(std::string volume, std::string directory, std::string filename);
private:
	static void TextareaEventHandler(lv_event_t* e);\
	std::string code;
};
