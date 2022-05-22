#include <iostream>
#include <thread>
#include "../OS/OS.h"
#include "../Tasks/WindowManager.h"
#include "../Tasks/DARICWindow.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Tester.h"
#include "../Applications/Clock.h"
#include "../Chrono/Chrono.h"
#include "../Tasks/Desktop.h"

extern int ScreenWidth;
extern int ScreenHeight;
size_t kernel_size = 0;
size_t pre_boot_memory = 0;

int main() {
    OS_Init();
    ClockInit();

    // What to run?
    auto gui = NEW WindowManager();
    DesktopStartup();
    gui->Start();
    gui->WaitForTermination();
    return 0;
}