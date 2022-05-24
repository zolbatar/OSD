#include <iostream>
#include <thread>
#include "../OS/OS.h"
#include "../Tasks/WindowManager/WindowManager.h"
#include "../Tasks/DARICWindow.h"
#include "../Chrono/Chrono.h"
#include "../Tasks/FontManager/FontManager.h"

extern int ScreenWidth;
extern int ScreenHeight;
size_t kernel_size = 0;
size_t pre_boot_memory = 0;

int main() {
    OS_Init();
    ClockInit();

    // What to run?
    auto fm = NEW FontManager();
    fm->InitFonts();
    auto gui = NEW WindowManager();
    gui->Start();
    gui->WaitForTermination();
    return 0;
}