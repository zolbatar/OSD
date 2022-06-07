#include <iostream>
#include <thread>
#include <fstream>
#include "../OS/OS.h"
#include "../Tasks/WindowManager/WindowManager.h"
#include "../Tasks/DARICWindow.h"
#include "../Chrono/Chrono.h"
#include "../Tasks/FontManager/FontManager.h"
#include "../Tokeniser/Tokeniser.h"

extern int ScreenWidth;
extern int ScreenHeight;
size_t kernel_size = 0;
size_t pre_boot_memory = 0;

//Tokeniser: 9.459000 millis
//Parser: 3.370000 millis
//IR Compiler: 2.258000 millis
//Native Compiler: 6.169000 millis
//Total: 21.793 millis

void Benchmark()
{
	OSDTask task;
	task.SetID("Benchmark");
	task.SetNameAndAddToList();
	std::string filename = ":SD.$.Welcome.Tester";
	auto code = task.LoadSource(filename);
	task.CompileSource(filename, &code);
}

int main()
{
	OS_Init();
	ClockInit();
	Tokeniser::Init();

	// What to run?
	Benchmark();
/*    auto fm = NEW FontManager();
    fm->InitFonts();
    auto gui = NEW WindowManager();
    gui->Start();
    gui->WaitForTermination();*/
	return 0;
}