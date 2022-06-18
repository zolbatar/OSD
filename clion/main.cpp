#include <iostream>
#include <thread>
#include <fstream>
#include "../OS/OS.h"
#include "../Tasks/WindowManager/WindowManager.h"
#include "../Tasks/DARICWindow.h"
#include "../Chrono/Chrono.h"
#include "../Tasks/FontManager/FontManager.h"
#include "../Tokeniser/Tokeniser.h"
#include "../Parser/Parser.h"

extern int ScreenWidth;
extern int ScreenHeight;
size_t kernel_size = 0;
size_t pre_boot_memory = 0;
WindowManager* gui = nullptr;

int main()
{
	OS_Init();
	ClockInit();
	Tokeniser::Init();
	FileManager::Init();
	Parser::Init();

	// What to run?
	auto fm = NEW FontManager();
	fm->InitFonts();
	gui = NEW WindowManager();
	gui->Start();

	auto tester = NEW DARICWindow("RayTracer", false, 100, 600, 400, 400);
	tester->LoadSourceCode(":SD.$.Welcome.Raytracer");
	tester->Run();

	return 0;
}