#pragma once
#include "../../GUI/Window/Window.h"
#include "../../GUI/lvgl.h"

class InputManager : public OSDTask {
public:
	InputManager();
	void Run();
	static void KeyDown(uint32_t key);
	static void KeyUp(uint32_t key);

private:
	static int KeyRISCOSTranslate(uint32_t key);

};
