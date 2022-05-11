#include <iostream>
#include <thread>
#include "../OS/OS.h"
#include "../Input/Input.h"
#include "../Editor/Editor.h"
#include "../Tasks/GUI.h"
#include "../Tasks/DARICWindow.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Tester.h"
#include "../Applications/Clock.h"
#include "../Chrono/Chrono.h"
#include "../Tasks/Desktop.h"

Input* input;
Editor* editor;
extern int ScreenWidth;
extern int ScreenHeight;

int main()
{
	OS_Init();
	ClockInit();
	input = NEW Input();

	// What to run?
	auto gui = NEW GUI();
	DesktopStartup();
	gui->Start();
	gui->WaitForTermination();
	return 0;
}