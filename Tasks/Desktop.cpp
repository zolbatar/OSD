#include "Desktop.h"
#include "../OS/OS.h"
#include "DARICWindow.h"
#include "TasksWindow.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Tester.h"
#include "../Applications/Clock.h"
#ifdef CLION
#include <thread>
#include <pthread.h>
#endif

void DesktopStartup()
{
#ifndef CLION
	auto mandelbrot = NEW DARICWindow("Mandelbrot", false, 100, 100, 400, 400);
	mandelbrot->SetSourceCode(DARIC_mandelbrot_single);
	mandelbrot->Start();

	auto mandelbrot2 = NEW DARICWindow("Mandelbrot", false, 400, 300, 500, 500);
	mandelbrot2->SetSourceCode(DARIC_mandelbrot_single);
	mandelbrot2->Start();

	auto clock = NEW DARICWindow("Clock", false, 800, 100, 400, 300);
	clock->SetSourceCode(DARIC_clock);
	clock->Start();

	auto tasks = NEW TasksWindow(1300, 400, 500, 500);
	tasks->Start();

//	delete these after termination?
#else
	auto tasks = NEW TasksWindow(300, 200, 400, 200);
	std::thread t1(&DARICWindow::Start, tasks);
	t1.detach();

/*    auto clock = NEW DARICWindow("Clock", false, 1000, 100, 400, 300);
    clock->SetSourceCode(DARIC_clock);
    std::thread t2(&DARICWindow::Start, clock);
    t2.detach();*/
#endif
}