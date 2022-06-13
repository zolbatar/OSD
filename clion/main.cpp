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
	Parser::Init();

	// What to run?
	auto fm = NEW FontManager();
	fm->InitFonts();
	gui = NEW WindowManager();
	gui->Start();
	return 0;
}