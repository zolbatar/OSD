#pragma once
#include "../OS/OS.h"
#include "../GUI/Window/Window.h"
#include <circle/logger.h>

class DARICWindow : public OSDTask {
public:
	DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h, int canvas_w, int canvas_h);
	void Run();
	void LoadSourceCode(std::string directory, std::string filename);
private:
	int canvas_w;
	int canvas_h;
	std::string filename;
	std::string code;

	void Maximise();
};
