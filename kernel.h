#pragma once
#include <circle_stdlib_app.h>
#include <circle/2dgraphics.h>
#include <circle/memory.h>
#include <circle/usb/usbmouse.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/usb/usbhcidevice.h>

class CKernel : public CStdlibAppStdio /*CStdlibAppNetwork*/
{
public:
	CKernel(void);
	TShutdownMode Run(void);

private:
	CMachineInfo mMachineInfo;
	CMemorySystem mMemory;
	CCPUThrottle mThrottle;
	CScheduler mScheduler;
};