#pragma once
//
// lvgl.h
//
// C++ wrapper for LVGL with mouse and touch screen support
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2019-2022  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <lvgl/lvgl/lvgl.h>
#include <circle/screen.h>
#include <circle/bcmframebuffer.h>
#include <circle/interrupt.h>
#include <circle/input/mouse.h>
#include <circle/dmachannel.h>
#include <circle/types.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/input/keyboardbuffer.h>
#include <assert.h>
#include <queue>

class GuiCLVGL {
public:
	GuiCLVGL(CScreenDevice* pScreen, CInterruptSystem* pInterrupt);
	GuiCLVGL(CBcmFrameBuffer* pFrameBuffer, CInterruptSystem* pInterrupt);
	~GuiCLVGL(void);
	bool Initialize(void);
	bool QuitRequested() { return quit_requested; }
	void Update(bool bPlugAndPlayUpdated = false);
	static void SetKeyboardGroup(lv_group_t* g);
	lv_indev_t* GetMouse() { return mouse; }
private:
	static void DisplayFlush(lv_disp_drv_t* pDriver, const lv_area_t* pArea, lv_color_t* pBuffer);
	static void DisplayFlushComplete(unsigned nChannel, bool bStatus, void* pParam);
	static void PointerRead(lv_indev_drv_t* pDriver, lv_indev_data_t* pData);
	static void KeyboardRead(lv_indev_drv_t* pDriver, lv_indev_data_t* pData);
	static void LogPrint(const char* pMessage);
	static void MouseEventHandler(TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove);
	static void MouseRemovedHandler(CDevice* pDevice, void* pContext);
	static void KeyboardEventHandler(const char* pString);
	static void KeyboardEventHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6]);
private:
	bool quit_requested = false;
	static lv_indev_t* mouse;
	static lv_indev_t* keyboard;
	lv_obj_t* mouse_cursor;

	unsigned m_nLastUpdate = 0;
	CMouseDevice* volatile m_pMouseDevice;
	CUSBKeyboardDevice* volatile m_pKeyboardDevice;
	lv_color_t* m_pBuffer1;
	lv_color_t* m_pBuffer2;
	CScreenDevice* m_pScreen;
	CBcmFrameBuffer* m_pFrameBuffer;
	CDMAChannel m_DMAChannel;
	lv_indev_data_t m_PointerData;
	static GuiCLVGL* s_pThis;
	static std::queue<uint32_t> keys;
	static uint32_t last_key;
};
