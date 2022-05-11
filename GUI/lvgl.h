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
#ifndef _lvgl_lvgl_h
#define _lvgl_lvgl_h

#ifdef CLION
#include <thread>
#include <lvgl.h>
#include <draw/sdl/lv_draw_sdl_utils.h>
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#include "TerminalOutput.h"
#else
#include <lvgl/lvgl/lvgl.h>
#endif
#ifndef CLION
#include <circle/screen.h>
#include <circle/bcmframebuffer.h>
#include <circle/interrupt.h>
#include <circle/input/mouse.h>
#include <circle/dmachannel.h>
#include <circle/types.h>
#endif

#include <assert.h>

class GuiCLVGL {
public:
#ifndef CLION
	GuiCLVGL(CScreenDevice* pScreen, CInterruptSystem* pInterrupt);
	GuiCLVGL(CBcmFrameBuffer* pFrameBuffer, CInterruptSystem* pInterrupt);
#else
	GuiCLVGL();
#endif
	~GuiCLVGL(void);

	bool Initialize(void);
	bool QuitRequested() { return quit_requested; }

#ifndef CLION
	void Update(bool bPlugAndPlayUpdated = false);
#else
	static lv_disp_t* DisplayInit(SDL_Window* window);
	void InputInit(bool use_mouse_cursor);
	static void MouseRead(lv_indev_drv_t* indev_drv_mouse, lv_indev_data_t* data);
	static void FlushCB(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* src);
	void ProcessEvents();
	void Update();
#endif
	lv_indev_t* GetMouse() { return mouse; }

private:
	static void DisplayFlush(lv_disp_drv_t* pDriver, const lv_area_t* pArea, lv_color_t* pBuffer);
	static void DisplayFlushComplete(unsigned nChannel, bool bStatus, void* pParam);
	static void PointerRead(lv_indev_drv_t* pDriver, lv_indev_data_t* pData);
	static void LogPrint(const char* pMessage);
#ifndef CLION
	static void MouseEventHandler(TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove);
	static void MouseRemovedHandler(CDevice* pDevice, void* pContext);
#endif
private:
	bool quit_requested = false;
	lv_indev_t* mouse;
	lv_indev_drv_t indev_drv_mouse;
	lv_obj_t* mouse_cursor;
#ifdef CLION
	static int mouse_x, mouse_y;
	static bool mouse_event;
	static bool mouse_pressed;
#endif

#ifndef CLION
	CScreenDevice* m_pScreen;
	CBcmFrameBuffer* m_pFrameBuffer;
	CDMAChannel m_DMAChannel;
	CMouseDevice* volatile m_pMouseDevice;
	lv_color_t* m_pBuffer1;
	lv_color_t* m_pBuffer2;
	unsigned m_nLastUpdate = 0;
#else
	float dpi_ratio;
	int desktop_screen_width, desktop_screen_height;
	SDL_Window* window;
#endif

	lv_indev_data_t m_PointerData;
	static GuiCLVGL* s_pThis;
};

#endif
