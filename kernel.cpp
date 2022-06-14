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
#include "../Chrono/Chrono.h"
#include "OS/OS.h"
#include "Tasks/FontManager/FontManager.h"
#include "Tasks/WindowManager/WindowManager.h"
#include "Tasks/DARICWindow.h"
#include "Tokeniser/Tokeniser.h"
#include "Parser/Parser.h"

size_t kernel_size = 0;
size_t initial_mem_free = 0;
size_t pre_boot_memory = 0;
int ScreenResX = 1920;
int ScreenResY = 1080;
size_t ScreenSize = 0;
CTimer* timer;
CBcmFrameBuffer* fb;
CScreenDevice* screen;
CMemorySystem* memory;
CInterruptSystem* interrupt;
CUSBHCIDevice* USBHCI;
CUserTimer* UserTimer;
const unsigned rate = USER_CLOCKHZ / 100;

#define NET_DEVICE_TYPE        NetDeviceTypeEthernet
//#define NET_DEVICE_TYPE        NetDeviceTypeWLAN

CKernel::CKernel(void)
//		:CStdlibAppNetwork("OS/D", CSTDLIBAPP_DEFAULT_PARTITION, 0, 0, 0, 0, NET_DEVICE_TYPE)
		:CStdlibAppStdio("OS/D"), mMulticore(&mMemory), mUserTimer(&mInterrupt, PeriodicHandler, this, false)
{
	timer = &mTimer;
	memory = &mMemory;
	screen = &mScreen;
	USBHCI = &mUSBHCI;
	interrupt = &mInterrupt;
	mThrottle.SetSpeed(TCPUSpeed::CPUSpeedMaximum);
	UserTimer = &mUserTimer;
}

CStdlibApp::TShutdownMode CKernel::Run(void)
{
	CString Message;
	if (!mScheduler.IsActive) {
		Message.Format("No scheduler available");
		mLogger.Write("OS/D", LogNotice, Message);
		while (1);
	}
	mScheduler.SuspendNewTasks();
	OSDTask::boot_task = mScheduler.GetCurrentTask();
	mScheduler.RegisterTaskSwitchHandler(OSDTask::TaskSwitchHandler);
	mScheduler.RegisterTaskTerminationHandler(OSDTask::TaskTerminationHandler);

#ifdef __arm__
	mLogger.Write(GetKernelName(), LogNotice, "Defined: ARM");
#endif
#ifdef __aarch64__
	mLogger.Write(GetKernelName(), LogNotice, "Defined: AARCH64");
#endif
	mLogger.Write(GetKernelName(), LogNotice, "Defined: RASPPI=%d", RASPPI);

	// OS Version
	Message.Format("Compile time: %s %s", __DATE__, __TIME__);
	mLogger.Write("OS/D", LogNotice, Message);

	// Hardware
	Message.Format("Machine: %s, RAM: %d MB", mMachineInfo.GetMachineName(), mMachineInfo.GetRAMSize());
	mLogger.Write("OS/D", LogNotice, Message);

	// Screen
	Message.Format("Screen: %dx%d/%dx%d", mScreen.GetWidth(), mScreen.GetHeight(), mScreen.GetColumns(),
			mScreen.GetRows());
	mLogger.Write("OS/D", LogNotice, Message);

	// Init clock and input stuff
	OS_Init();
	ClockInit();
	Tokeniser::Init();
	Parser::Init();

	/*	CString IPString;
	mNet.GetConfig()->GetIPAddress()->Format(&IPString);
	mLogger.Write(GetKernelName(), LogNotice, "Try \"ping %s\" from another computer!", (const char*)IPString);
	mTimer.SetTimeZone(0);
	mTimer.MsDelay(20000);*/

	// Initial memory and kernel size
#ifndef CLION
	auto memory = CMemorySystem::Get();
	initial_mem_free = memory->GetHeapFreeSpace(HEAP_ANY);
	//kernel_size = memory->GetMemSize()-initial_mem_free;
#else
	initial_mem_free = 1;
	kernel_size = 1;
#endif

	// Now fire cores up
	mMulticore.Initialize();

	// Start the pre-emptive
	mUserTimer.Initialize();
	mUserTimer.Start(rate);

	// Font manager if always first
	auto fm = new FontManager();
	fm->InitFonts();
	fm->Start();

	// Wait for GUI startup
	auto gui = new WindowManager();
	gui->Start();

	while (1) { // Wait forever for now, no shutdown procedure
		CScheduler::Get()->Yield();
	}

	mLogger.Write("OS/D", LogNotice, "Termination");

	return ShutdownHalt;
}

CMultiCore::CMultiCore(CMemorySystem* pMemorySystem)
		:CMultiCoreSupport(pMemorySystem)
{
}

void CMultiCore::Run(unsigned nCore)
{
	switch (nCore) {
		case 1:
			break;
	}
}

void CKernel::PeriodicHandler(CUserTimer* pTimer, void* pParam)
{
//	CLogger::Get()->Write("OS/D", LogNotice, "Tick");
	auto task = GetCurrentTask();
	if (OSDTask::inside_api) {
		OSDTask::SetDelayedYield();
		return;
	}
	task->Yield();
}
