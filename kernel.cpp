#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "kernel.h"
#ifndef CLION
#include <circle/new.h>
#endif
#include "Input/Input.h"
#include "Chrono/Chrono.h"
#include "OS/OS.h"
#include "Tasks/GUI.h"
#include "Tasks/DARICWindow.h"
#include "Tasks/Desktop.h"

size_t kernel_size = 0;
size_t initial_mem_free = 0;
int ScreenResX = 1920;
int ScreenResY = 1080;
size_t ScreenSize = 0;
CTimer* timer;
CBcmFrameBuffer* fb;
Input* input;
CScreenDevice* screen;
CMemorySystem* memory;
CInterruptSystem* interrupt;
CUSBHCIDevice* USBHCI;

#define NET_DEVICE_TYPE        NetDeviceTypeEthernet
//#define NET_DEVICE_TYPE        NetDeviceTypeWLAN

CKernel::CKernel(void)
//		:CStdlibAppNetwork("OS/D", CSTDLIBAPP_DEFAULT_PARTITION, 0, 0, 0, 0, NET_DEVICE_TYPE)
		:CStdlibAppStdio("OS/D")
{
	timer = &mTimer;
	memory = &mMemory;
	screen = &mScreen;
	USBHCI = &mUSBHCI;
	interrupt = &mInterrupt;
	mThrottle.SetSpeed(TCPUSpeed::CPUSpeedMaximum);
}

CStdlibApp::TShutdownMode CKernel::Run(void)
{
	// Initial memory and kernel size
	initial_mem_free = mMemory.GetHeapFreeSpace(HEAP_ANY);
	kernel_size = mMemory.GetMemSize()-initial_mem_free;

	CString Message;

#ifdef __arm__
	mLogger.Write(GetKernelName(), LogNotice, "Defined: ARM");
#endif
#ifdef __aarch64__
	mLogger.Write(GetKernelName(), LogNotice, "Defined: AARCH64");
#endif
	mLogger.Write(GetKernelName(), LogNotice, "Defined: RASPPI=%d", RASPPI);

	// OS Version
	Message.Format("Compile time: %s %s", __DATE__, __TIME__);
	mLogger.Write("OS/D", LogDebug, Message);

	// Hardware
	Message.Format("Machine: %s, RAM: %d MB", mMachineInfo.GetMachineName(), mMachineInfo.GetRAMSize());
	mLogger.Write("OS/D", LogDebug, Message);

	// Screen
	Message.Format("Screen: %dx%d/%dx%d", mScreen.GetWidth(), mScreen.GetHeight(), mScreen.GetColumns(),
			mScreen.GetRows());
	mLogger.Write("OS/D", LogDebug, Message);

	if (!mScheduler.IsActive) {
		Message.Format("No scheduler available");
		mLogger.Write("OS/D", LogDebug, Message);
		while (1);
	}
	mScheduler.SuspendNewTasks();

/*	CString IPString;
	mNet.GetConfig()->GetIPAddress()->Format(&IPString);
	mLogger.Write(GetKernelName(), LogNotice, "Try \"ping %s\" from another computer!", (const char*)IPString);
	mTimer.SetTimeZone(0);
	mTimer.MsDelay(20000);*/

	// Framebuffer
	OS_Init();

	// Init clock and input stuff
	ClockInit();
	input = NEW Input();

	// What to run?
	auto gui = NEW GUI();
	gui->Start();
	DesktopStartup();
	gui->WaitForTermination();

	return ShutdownHalt;
}