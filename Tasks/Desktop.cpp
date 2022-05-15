#include "Desktop.h"
#include "../OS/OS.h"
#include "DARICWindow.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Tester.h"
#include "../Applications/Clock.h"
#ifdef CLION
#include <thread>
#include <pthread.h>
#endif

typedef void* (* THREADFUNCPTR)(void*);

void DesktopStartup()
{
	auto mandelbrot = NEW DARICWindow("MANDELBROT", "Mandelbrot", false, 100, 100, 400, 400);
	mandelbrot->SetSourceCode(DARIC_mandelbrot);
	mandelbrot->Start();

	auto mandelbrot2 = NEW DARICWindow("MANDELBROT2", "Mandelbrot", false, 400, 300, 500, 500);
	mandelbrot2->SetSourceCode(DARIC_mandelbrot_single);
	mandelbrot2->Start();

	auto clock = NEW DARICWindow("CLOCK", "Clock", false, 1000, 100, 400, 300);
	clock->SetSourceCode(DARIC_clock);
	clock->Start();
}