#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "kernel.h"
#include <circle/new.h>
#include "OS/Chrono/Chrono.h"
#include "OS/OS.h"
#include "Tasks/System/FontManager/FontManager.h"
#include "Tasks/System/FileManager/FileManager.h"
#include "Tasks/System/IconBar/IconBar.h"
#include "Tasks/System/WindowManager/WindowManager.h"
#include "Tasks/System/InputManager/InputManager.h"
#include "Tasks/DARICWindow.h"
#include "Compiler/Tokeniser/Tokeniser.h"
#include "Compiler/Parser/Parser.h"

size_t kernel_size = 0;
size_t initial_mem_free = 0;
size_t pre_boot_memory = 0;
size_t ScreenSize = 0;
CTimer *timer;
CBcmFrameBuffer *fb;
CScreenDevice *screen;
CMemorySystem *memory;
CInterruptSystem *interrupt;
CUSBHCIDevice *USBHCI;
CUserTimer *UserTimer;
unsigned rate = USER_CLOCKHZ;
FontManager *fm;

#define NET_DEVICE_TYPE NetDeviceTypeEthernet
//#define NET_DEVICE_TYPE        NetDeviceTypeWLAN

CKernel::CKernel(void)
    //		:CStdlibAppNetwork("Daric", CSTDLIBAPP_DEFAULT_PARTITION, 0, 0, 0, 0, NET_DEVICE_TYPE)
    : CStdlibAppStdio("Daric"), mMulticore(&mMemory), mUserTimer(&mInterrupt, PeriodicHandler, this, false)
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
    if (!mScheduler.IsActive)
    {
        Message.Format("No scheduler available");
        mLogger.Write("Daric", LogNotice, Message);
        while (1)
            ;
    }
    mScheduler.SuspendNewTasks();
    OSDTask::boot_task = mScheduler.GetCurrentTask();
    mScheduler.RegisterTaskSwitchHandler(OSDTask::TaskSwitchHandler);
    mScheduler.RegisterTaskTerminationHandler(OSDTask::TaskTerminationHandler);

    // Hardware
    Message.Format("Machine: %s, RAM: %d MB", mMachineInfo.GetMachineName(), mMachineInfo.GetRAMSize());
    mLogger.Write("Daric", LogNotice, Message);

    // Screen
    Message.Format("Screen: %dx%d/%dx%d", mScreen.GetWidth(), mScreen.GetHeight(), mScreen.GetColumns(),
                   mScreen.GetRows());
    mLogger.Write("Daric", LogNotice, Message);

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
    // auto memory = CMemorySystem::Get();
    initial_mem_free = memory->GetHeapFreeSpace(HEAP_ANY);
    kernel_size = memory->GetMemSize() - initial_mem_free;

    // Now fire cores up
    //	mMulticore.Initialize();

    // Start the pre-emptive
    mUserTimer.Initialize();
    mUserTimer.Start(rate);

    // File manager is always first, then other system services
    auto fim = new FileManager();
    fim->Start();
    CScheduler::Get()->Yield();
    fm = new FontManager();
    fm->InitFonts();
    fm->Start();
    CScheduler::Get()->Yield();
    auto im = new InputManager();
    im->Start();
    CScheduler::Get()->Yield();

    // Wait for GUI startup
    auto gui = new WindowManager();
    gui->Start();
    CScheduler::Get()->Yield();

    // Icon Bar
    auto ib = new IconBar();
    ib->Run();

    while (1)
    { // Wait forever for now, no shutdown procedure
        CScheduler::Get()->Yield();
    }

    mLogger.Write("Daric", LogNotice, "Termination");

    return ShutdownHalt;
}

CMultiCore::CMultiCore(CMemorySystem *pMemorySystem) : CMultiCoreSupport(pMemorySystem)
{
}

void CMultiCore::Run(unsigned nCore)
{
    switch (nCore)
    {
    case 1:
        break;
    }
}

void CKernel::PeriodicHandler(CUserTimer *pTimer, void *pParam)
{
    OSDTask::yield_due = true;
}
