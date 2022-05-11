#include "Desktop.h"
#include "../OS/OS.h"
#include "DARICWindow.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Tester.h"
#include "../Applications/Clock.h"

void DesktopStartup()
{
	auto mandelbrot = NEW DARICWindow("MANDELBROT", "Mandelbrot", false, 100, 100, 400, 400);
	mandelbrot->SetSourceCode(DARIC_mandelbrot);
#ifndef CLION
	mandelbrot->Start();
#else
	auto t1 = std::thread(&DARICWindow::Start, mandelbrot);
	t1.detach();
#endif

	auto mandelbrot2 = NEW DARICWindow("MANDELBROT2", "Mandelbrot", false, 400, 300, 500, 500);
	mandelbrot2->SetSourceCode(DARIC_mandelbrot_single);
#ifndef CLION
	mandelbrot2->Start();
#else
	auto t2 = std::thread(&DARICWindow::Start, mandelbrot2);
	t2.detach();
#endif

	auto clock = NEW DARICWindow("CLOCK", "Clock", false, 1000, 100, 400, 300);
	clock->SetSourceCode(DARIC_clock);
#ifndef CLION
	clock->Start();
#else
	auto t3 = std::thread(&DARICWindow::Start, clock);
	t3.detach();
#endif
}