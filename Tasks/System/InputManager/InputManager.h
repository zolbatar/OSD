#pragma once
#include "InputManagerStruct.h"
#include "../../GUI/Window/Window.h"
#include "../../GUI/lvgl.h"

class InputManager : public OSDTask {
public:
	InputManager();
	void Run();
	static void KeyDown(uint32_t key);
	static void KeyUp(uint32_t key);
	static void ClaimInput(OSDTask* task);

private:
	static int KeyRISCOSTranslate(uint32_t key);
	static OSDTask* current_task;
};
