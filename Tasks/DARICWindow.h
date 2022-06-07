#pragma once
#include "../OS/OS.h"
#include "../GUI/Window.h"
#ifndef CLION
#include <circle/logger.h>
#endif

class DARICWindow : public OSDTask {
public:
	DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h);
	void Run();
	void LoadSourceCode(std::string filename);
	void SetSourceCode(std::string code);
};
