#pragma once
#include "WindowManager/WindowManager.h"

class DARICWindow : public OSDTask {
public:
	DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h);
	void Run();
	void SetSourceCode(std::string code);
private:
	std::string code;
};
