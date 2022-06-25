//
// lvgl.cpp
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
#include "lvgl.h"
#include <circle/devicenameservice.h>
#include <circle/koptions.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/string.h>
#include <circle/util.h>
#include <circle/new.h>
#include <string>
#include "KeyboardCodes.h"
#include "../Tasks/InputManager/InputManager.h"

GuiCLVGL* GuiCLVGL::s_pThis = 0;
std::queue<uint32_t> GuiCLVGL::keys;
uint32_t GuiCLVGL::last_key = 0;
lv_indev_t* GuiCLVGL::mouse;
lv_indev_t* GuiCLVGL::keyboard;

GuiCLVGL::GuiCLVGL(CScreenDevice* pScreen, CInterruptSystem* pInterrupt)
		:m_pBuffer1(0), m_pBuffer2(0), m_pScreen(pScreen), m_pFrameBuffer(0), m_DMAChannel(DMA_CHANNEL_NORMAL, pInterrupt),
		 m_nLastUpdate(0), m_pMouseDevice(0)
{
	assert(s_pThis==0);
	s_pThis = this;

	m_PointerData.state = LV_INDEV_STATE_REL;
	m_PointerData.point.x = 0;
	m_PointerData.point.y = 0;
}

GuiCLVGL::GuiCLVGL(CBcmFrameBuffer* pFrameBuffer, CInterruptSystem* pInterrupt)
		:m_pBuffer1(0), m_pBuffer2(0), m_pScreen(0), m_pFrameBuffer(pFrameBuffer),
		 m_DMAChannel(DMA_CHANNEL_NORMAL, pInterrupt), m_nLastUpdate(0), m_pMouseDevice(0)
{
	assert(s_pThis==0);
	s_pThis = this;

	m_PointerData.state = LV_INDEV_STATE_REL;
	m_PointerData.point.x = 0;
	m_PointerData.point.y = 0;
}

GuiCLVGL::~GuiCLVGL(void)
{
	s_pThis = 0;

	m_pMouseDevice = 0;
	m_pFrameBuffer = 0;
	m_pScreen = 0;
	delete[] m_pBuffer1;
	delete[] m_pBuffer2;
	m_pBuffer1 = 0;
	m_pBuffer2 = 0;
}

bool GuiCLVGL::Initialize(void)
{
	if (m_pFrameBuffer==0) {
		assert(m_pScreen!=0);
		m_pFrameBuffer = m_pScreen->GetFrameBuffer();
	}

	assert(m_pFrameBuffer!=0);
	assert(m_pFrameBuffer->GetDepth()==LV_COLOR_DEPTH);
	size_t nWidth = m_pFrameBuffer->GetWidth();
	size_t nHeight = m_pFrameBuffer->GetHeight();
	CLogger::Get()->Write("lvgl", LogNotice, "Resolution: %dx%d", nWidth, nHeight);

	lv_init();

	lv_log_register_print_cb(LogPrint);

	m_pBuffer1 = new(HEAP_DMA30) lv_color_t[nWidth*10];
	m_pBuffer2 = new(HEAP_DMA30) lv_color_t[nWidth*10];
	if (m_pBuffer1==0 || m_pBuffer2==0) {
		return false;
	}

	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, m_pBuffer1, m_pBuffer2, nWidth*10);

	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = DisplayFlush;
	disp_drv.hor_res = nWidth;
	disp_drv.ver_res = nHeight;
	lv_disp_drv_register(&disp_drv);

	// Mouse
	m_pMouseDevice = (CMouseDevice*)CDeviceNameService::Get()->GetDevice("mouse1", FALSE);
	if (m_pMouseDevice!=0) {
		if (m_pMouseDevice->Setup(nWidth, nHeight)) {
			m_pMouseDevice->ShowCursor(FALSE);
			m_pMouseDevice->RegisterEventHandler(MouseEventHandler);
			m_pMouseDevice->RegisterRemovedHandler(MouseRemovedHandler);
		}
		else {
			m_pMouseDevice = 0;
		}
	}
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = PointerRead;
	mouse = lv_indev_drv_register(&indev_drv);

	// Keyboard
	m_pKeyboardDevice = (CUSBKeyboardDevice*)CDeviceNameService::Get()->GetDevice("ukbd1", FALSE);
	if (m_pKeyboardDevice!=0) {
		m_pKeyboardDevice->RegisterKeyPressedHandler(KeyboardEventHandler);
		m_pKeyboardDevice->RegisterKeyStatusHandlerRaw(KeyboardEventHandlerRaw, TRUE);
	}
	static lv_indev_drv_t indev_drvk;
	lv_indev_drv_init(&indev_drvk);
	indev_drvk.type = LV_INDEV_TYPE_KEYPAD;
	indev_drvk.read_cb = KeyboardRead;
	keyboard = lv_indev_drv_register(&indev_drvk);

	// Keyboard group
	//	static lv_group_t* g = lv_group_create();
//	lv_group_set_default(g);
//	lv_indev_set_group(keyboard, g);

	return true;
}

void GuiCLVGL::SetKeyboardGroup(lv_group_t* g)
{
	lv_indev_set_group(keyboard, g);
}

void GuiCLVGL::Update(bool bPlugAndPlayUpdated)
{
	if (bPlugAndPlayUpdated && m_pMouseDevice==0) {
		m_pMouseDevice = (CMouseDevice*)CDeviceNameService::Get()->GetDevice("mouse1", FALSE);
		if (m_pMouseDevice!=0) {
			assert(m_pFrameBuffer!=0);
			if (m_pMouseDevice->Setup(m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight())) {
				m_pMouseDevice->ShowCursor(FALSE);
				m_pMouseDevice->RegisterEventHandler(MouseEventHandler);
				m_pMouseDevice->RegisterRemovedHandler(MouseRemovedHandler);
			}
		}
	}

	lv_task_handler();

	unsigned nTicks = CTimer::Get()->GetClockTicks();
	if (nTicks-m_nLastUpdate>=CLOCKHZ/1000) {
		lv_tick_inc((nTicks-m_nLastUpdate)/(CLOCKHZ/1000));

		m_nLastUpdate = nTicks;
	}

	if (m_pMouseDevice!=0) {
		m_pMouseDevice->UpdateCursor();
	}
}

void GuiCLVGL::DisplayFlush(lv_disp_drv_t* pDriver, const lv_area_t* pArea, lv_color_t* pBuffer)
{
	assert(s_pThis!=0);

	assert(pArea!=0);
	int32_t x1 = pArea->x1;
	int32_t x2 = pArea->x2;
	int32_t y1 = pArea->y1;
	int32_t y2 = pArea->y2;

	assert(x1<=x2);
	assert(y1<=y2);
	assert(pBuffer!=0);

	assert(s_pThis->m_pFrameBuffer!=0);
	void* pDestination = (void*)(uintptr)(s_pThis->m_pFrameBuffer->GetBuffer()+
			y1*s_pThis->m_pFrameBuffer->GetPitch()+x1*LV_COLOR_DEPTH/8);

	size_t nBlockLength = (x2-x1+1)*LV_COLOR_DEPTH/8;

	s_pThis->m_DMAChannel.SetupMemCopy2D(pDestination, pBuffer, nBlockLength, y2-y1+1,
			s_pThis->m_pFrameBuffer->GetPitch()-nBlockLength);

	assert(pDriver!=0);
	s_pThis->m_DMAChannel.SetCompletionRoutine(DisplayFlushComplete, pDriver);
	s_pThis->m_DMAChannel.Start();
}

void GuiCLVGL::DisplayFlushComplete(unsigned nChannel, bool bStatus, void* pParam)
{
	assert(bStatus);

	lv_disp_drv_t* pDriver = (lv_disp_drv_t*)pParam;
	assert(pDriver!=0);

	lv_disp_flush_ready(pDriver);
}

void GuiCLVGL::PointerRead(lv_indev_drv_t* pDriver, lv_indev_data_t* pData)
{
	assert(s_pThis!=0);

	pData->point.x = s_pThis->m_PointerData.point.x;
	pData->point.y = s_pThis->m_PointerData.point.y;
	pData->state = s_pThis->m_PointerData.state;
}

void GuiCLVGL::KeyboardRead(lv_indev_drv_t* pDriver, lv_indev_data_t* pData)
{
	assert(s_pThis!=0);
	if (keys.empty())
		return;

	if (last_key!=0) {
		// Do release
		InputManager::KeyUp(last_key);
		pData->key = last_key;
		pData->state = LV_INDEV_STATE_RELEASED;
		last_key = 0;
	}
	else {
		// Get key
		auto t = keys.front();
		keys.pop();

		if (t==KEY_Left) {
			t = LV_KEY_LEFT;
		}
		else if (t==KEY_Right) {
			t = LV_KEY_RIGHT;
		}
		else if (t==KEY_Up) {
			t = LV_KEY_UP;
		}
		else if (t==KEY_Down) {
			t = LV_KEY_DOWN;
		}
		else if (t==13) {
			t = LV_KEY_ENTER;
		}
		else if (t==KEY_ShiftTab) {
			t = LV_KEY_PREV;
		}
		else if (t==KEY_Tab) {
			t = LV_KEY_NEXT;
		}

		InputManager::KeyDown(t);
		pData->key = t;
		pData->state = LV_INDEV_STATE_PRESSED;
		last_key = t;
	}

	if (!keys.empty())
		pData->continue_reading = true;
}

void GuiCLVGL::MouseEventHandler(TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove)
{
	assert(s_pThis!=0);
	s_pThis->m_PointerData.point.x = nPosX;
	s_pThis->m_PointerData.point.y = nPosY;

	switch (Event) {
		case MouseEventMouseMove:
		case MouseEventMouseDown:
			if (nButtons & MOUSE_BUTTON_LEFT) {
				s_pThis->m_PointerData.state = LV_INDEV_STATE_PR;
			}
			break;

		case MouseEventMouseUp:
			if (nButtons & MOUSE_BUTTON_LEFT) {
				s_pThis->m_PointerData.state = LV_INDEV_STATE_REL;
			}
			break;

		default:
			break;
	}
}

void GuiCLVGL::KeyboardEventHandler(const char* pString)
{
	auto cc = pString[0];
	auto icc = int(cc);
	switch (icc) {
		case 27: { // Escape
			std::string s(&pString[1]);
			if (s=="")
				keys.push(KEY_Escape);
			else if (s=="[2~")
				keys.push(KEY_Insert);
			else if (s=="[1~")
				keys.push(KEY_Home);
			else if (s=="[5~")
				keys.push(KEY_PageUp);
			else if (s=="[3~")
				keys.push(KEY_Delete);
			else if (s=="[4~")
				keys.push(KEY_End);
			else if (s=="[6~")
				keys.push(KEY_PageDown);
			else if (s=="[A")
				keys.push(KEY_Up);
			else if (s=="[B")
				keys.push(KEY_Down);
			else if (s=="[D")
				keys.push(KEY_Left);
			else if (s=="[C")
				keys.push(KEY_Right);
			else if (s=="[[A")
				keys.push(KEY_F1);
			else if (s=="[[B")
				keys.push(KEY_F2);
			else if (s=="[[C")
				keys.push(KEY_F3);
			else if (s=="[[D")
				keys.push(KEY_F4);
			else if (s=="[[E")
				keys.push(KEY_F5);
			else if (s=="[17~")
				keys.push(KEY_F6);
			else if (s=="[18~")
				keys.push(KEY_F7);
			else if (s=="[19~")
				keys.push(KEY_F8);
			else if (s=="[20~")
				keys.push(KEY_F9);
			else if (s=="[G")
				keys.push(KEY_Center);
			else
				CLogger::Get()->Write("Keyboard", LogNotice, "Escape: '%s'", s.c_str());
			break;
		}
		case 127: // backspace
			keys.push(KEY_Backspace);
			break;
		default:
			// Regular key
			keys.push(icc);
//			CLogger::Get()->Write("Keyboard", LogNotice, "%d", icc);
			break;

	}
}

void GuiCLVGL::KeyboardEventHandlerRaw(unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	if (ucModifiers & KEY_LWIN_MASK || ucModifiers & KEY_RWIN_MASK) {
		keys.push(KEY_WindowsKey);
	}
	else if (ucModifiers==0 && RawKeys[0]==43) {
		keys.push(KEY_Tab);
	}
	else if (ucModifiers & KEY_LSHIFT_MASK==KEY_LSHIFT_MASK && RawKeys[0]==43) {
		keys.push(KEY_ShiftTab);
	}
	else if (ucModifiers & KEY_ALT_MASK==KEY_ALT_MASK && RawKeys[0]==43) {
		keys.push(KEY_AltTab);
	}
	if ((int)RawKeys[0]==0)
		return;
/*	CLogger::Get()
			->Write("lvgl", LogDebug, "%d %d/%d/%d/%d/%d/%d", ucModifiers & KEY_ALT_MASK, (int)RawKeys[0], (int)RawKeys[1], (int)RawKeys[2], (int)RawKeys[3],
					(int)RawKeys[4], (int)RawKeys[5]);*/
}

void GuiCLVGL::MouseRemovedHandler(CDevice* pDevice, void* pContext)
{
	assert(s_pThis!=0);
	s_pThis->m_pMouseDevice = 0;
}

void GuiCLVGL::LogPrint(const char* pMessage)
{
	assert(pMessage!=0);
	size_t nLen = strlen(pMessage);
	assert(nLen>0);

	char Buffer[nLen+1];
	strcpy(Buffer, pMessage);

	if (Buffer[nLen-1]=='\n') {
		Buffer[nLen-1] = '\0';
	}

	CLogger::Get()->Write("lvgl", LogDebug, Buffer);
}
