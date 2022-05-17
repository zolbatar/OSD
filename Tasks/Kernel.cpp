#include "Kernel.h"
#include "../Chrono/Chrono.h"
#include "../OS/OS.h"
#include "GUI.h"
#include "Desktop.h"
#ifndef CLION
#include <circle/new.h>
#include <circle/logger.h>
#endif

size_t kernel_size = 0;
size_t initial_mem_free = 0;
extern void ProcessAllocList();

Kernel::Kernel()
{
	this->name = "Kernel Supervisor	";
	SetNameAndAddToList();

	// Init clock and input stuff
	OS_Init();
	ClockInit();
}

void Kernel::Run()
{
	// Initial memory and kernel size
#ifndef CLION
    auto memory = CMemorySystem::Get();
	initial_mem_free = memory->GetHeapFreeSpace(HEAP_ANY);
	kernel_size = memory->GetMemSize()-initial_mem_free;
#else
	initial_mem_free = 1;
	kernel_size = 1;
#endif

	// Start
	GUI gui;
	gui.Start();
	DesktopStartup();

	// Forever loop
	while (1) {
		Yield();
	}

	gui.WaitForTermination();
}