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

#ifndef CLION
#include <circle/devicenameservice.h>
#include <circle/koptions.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/string.h>
#include <circle/util.h>
#include <circle/new.h>
#else
#include <stdio.h>
bool GuiCLVGL::mouse_event = false;
bool GuiCLVGL::mouse_pressed = false;
int GuiCLVGL::mouse_x;
int GuiCLVGL::mouse_y;

#ifdef __aarch64__
int ScreenWidth = 1280;
int ScreenHeight = 720;
#else
int ScreenWidth = 1920;
int ScreenHeight = 1080;
#endif
#endif

GuiCLVGL* GuiCLVGL::s_pThis = 0;

#ifndef CLION
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
#else

GuiCLVGL::GuiCLVGL()
{
	assert(s_pThis==0);
	s_pThis = this;

	m_PointerData.state = LV_INDEV_STATE_REL;
	m_PointerData.point.x = 0;
	m_PointerData.point.y = 0;
}

#endif

GuiCLVGL::~GuiCLVGL(void)
{
	s_pThis = 0;

#ifndef CLION
	m_pMouseDevice = 0;
	m_pFrameBuffer = 0;
	m_pScreen = 0;
	delete[] m_pBuffer1;
	delete[] m_pBuffer2;
	m_pBuffer1 = 0;
	m_pBuffer2 = 0;
#else
	TerminalShutdown("SDL/UI");
//    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
#endif
}

bool GuiCLVGL::Initialize(void)
{
#ifndef CLION
	if (m_pFrameBuffer == 0) {
		assert(m_pScreen != 0);
		m_pFrameBuffer = m_pScreen->GetFrameBuffer();
	}

	assert(m_pFrameBuffer != 0);
	assert(m_pFrameBuffer->GetDepth() == LV_COLOR_DEPTH);
	size_t nWidth = m_pFrameBuffer->GetWidth();
	size_t nHeight = m_pFrameBuffer->GetHeight();

	lv_init();

	lv_log_register_print_cb(LogPrint);

	m_pBuffer1 = new(HEAP_DMA30) lv_color_t[nWidth * 10];
	m_pBuffer2 = new(HEAP_DMA30) lv_color_t[nWidth * 10];
	if (m_pBuffer1 == 0 || m_pBuffer2 == 0) {
		return false;
	}

	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, m_pBuffer1, m_pBuffer2, nWidth * 10);

	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = DisplayFlush;
	disp_drv.hor_res = nWidth;
	disp_drv.ver_res = nHeight;
	lv_disp_drv_register(&disp_drv);

	m_pMouseDevice = (CMouseDevice *) CDeviceNameService::Get()->GetDevice("mouse1", FALSE);
	CString Message;

	if (m_pMouseDevice != 0) {
		if (m_pMouseDevice->Setup(nWidth, nHeight)) {
			m_pMouseDevice->ShowCursor(FALSE);
			m_pMouseDevice->RegisterEventHandler(MouseEventHandler);
			m_pMouseDevice->RegisterRemovedHandler(MouseRemovedHandler);
		} else {
			m_pMouseDevice = 0;
		}
	}

	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = PointerRead;
	mouse = lv_indev_drv_register(&indev_drv);

	return true;
#else
	TerminalInit("SDL/UI");
	if (SDL_Init(SDL_INIT_EVERYTHING)<0) {
		std::cout << "SDL initialization failed. SDL Error: " << SDL_GetError() << std::endl;
	}

	float ddpi, hdpi, vdpi;
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)!=0) {
		std::cout << bright_red << "ERROR: " << bright_white << "Failed to obtain DPI information for display 0: \n"
				  << SDL_GetError() << reset << std::endl;
		exit(1);
	}
#ifdef __APPLE__
	dpi_ratio = ddpi/96;
#else
	dpi_ratio = ddpi/72;
#endif
	lv_init();

	dpi_ratio = roundf(dpi_ratio);
	std::cout << white << "DPI ratio: " << bright_yellow << dpi_ratio << std::endl;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm)!=0) {
		std::cout << bright_red << "ERROR: " << bright_white << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError()
				  << reset << std::endl;
		exit(1);
	}
	desktop_screen_width = dm.w;
	desktop_screen_height = dm.h;
	std::cout << white << "Desktop resolution: " << bright_yellow << desktop_screen_width << white << "x"
			  << bright_yellow
			  << desktop_screen_height << reset << std::endl;

	// Create window
	TerminalOut("Creating", "SDL window");
	SDL_WindowFlags
			window_flags = (SDL_WindowFlags)(/*SDL_WINDOW_BORDERLESS*/0);
	window = SDL_CreateWindow("OS/D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight,
			window_flags);
	if (window==nullptr) {
		std::cout << bright_red << "ERROR: " << bright_white << "Window could not be created. SDL_Error: "
				  << SDL_GetError()
				  << reset << std::endl;
		exit(1);
	}

	lv_disp_t* disp = DisplayInit(window);
	lv_disp_set_default(disp);
	InputInit(true);

	return true;
#endif
}

#ifndef CLION
void GuiCLVGL::Update(bool bPlugAndPlayUpdated) {
	if (bPlugAndPlayUpdated && m_pMouseDevice == 0) {
		m_pMouseDevice = (CMouseDevice *) CDeviceNameService::Get()->GetDevice("mouse1", FALSE);
		if (m_pMouseDevice != 0) {
			assert(m_pFrameBuffer != 0);
			if (m_pMouseDevice->Setup(m_pFrameBuffer->GetWidth(), m_pFrameBuffer->GetHeight())) {
				m_pMouseDevice->ShowCursor(FALSE);
				m_pMouseDevice->RegisterEventHandler(MouseEventHandler);
				m_pMouseDevice->RegisterRemovedHandler(MouseRemovedHandler);
			}
		}
	}

	lv_task_handler();

	unsigned nTicks = CTimer::Get()->GetClockTicks();
	if (nTicks - m_nLastUpdate >= CLOCKHZ / 1000) {
		lv_tick_inc((nTicks - m_nLastUpdate) / (CLOCKHZ / 1000));

		m_nLastUpdate = nTicks;
	}

	if (m_pMouseDevice != 0) {
		m_pMouseDevice->UpdateCursor();
	}
}

#else

lv_disp_t* GuiCLVGL::DisplayInit(SDL_Window* window)
{
	int width, height;

	// Setup SDL
	SDL_GetWindowSize(window, &width, &height);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Setup draw buffer
	static lv_disp_draw_buf_t draw_buf;
	SDL_Texture* texture = (SDL_Texture*)lv_draw_sdl_create_screen_texture(renderer, width, height);
	lv_disp_draw_buf_init(&draw_buf, texture, NULL, width*height);

	// Display driver
	static lv_disp_drv_t driver;
	lv_disp_drv_init(&driver);

	static lv_draw_sdl_drv_param_t param;
	param.renderer = renderer;
	driver.user_data = &param;
	driver.draw_buf = &draw_buf;
	driver.flush_cb = FlushCB;
	driver.clear_cb = ClearCB;
	driver.hor_res = width;
	driver.ver_res = height;
	SDL_SetRenderTarget(renderer, texture);
	lv_disp_t* disp = lv_disp_drv_register(&driver);
	lv_disp_set_default(disp);
	return disp;
}

void GuiCLVGL::InputInit(bool use_mouse_cursor)
{
	// Register a mouse cursor
	if (use_mouse_cursor) {
		lv_indev_drv_init(&indev_drv_mouse);
		indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
		indev_drv_mouse.read_cb = MouseRead;
		mouse = lv_indev_drv_register(&indev_drv_mouse);
/*		mouse_cursor = lv_img_create(lv_scr_act());
		lv_img_set_src(mouse_cursor, LV_SYMBOL_PLUS);
		lv_indev_set_cursor(mouse, mouse_cursor);*/
	}
}

void GuiCLVGL::MouseRead(lv_indev_drv_t* indev_drv_mouse, lv_indev_data_t* data)
{
	data->state = (mouse_pressed) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;

	// Event for a USB mouse
	if (mouse_event) {
		uint32_t buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
		data->point.x = mouse_x;
		data->point.y = mouse_y;
		data->state = (buttons & SDL_BUTTON_LMASK) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
		mouse_event = false;
	}
}

void GuiCLVGL::ClearCB(lv_disp_drv_t* disp_drv, uint8_t* buf, uint32_t size)
{

}

void GuiCLVGL::FlushCB(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* src)
{
	LV_UNUSED(src);
	if (area->x2<0 || area->y2<0 ||
			area->x1>disp_drv->hor_res-1 || area->y1>disp_drv->ver_res-1) {
		lv_disp_flush_ready(disp_drv);
		return;
	}

	if (lv_disp_flush_is_last(disp_drv)) {
		lv_draw_sdl_drv_param_t* param = (lv_draw_sdl_drv_param_t*)disp_drv->user_data;
		SDL_Renderer* renderer = param->renderer;
		SDL_Texture* texture = (SDL_Texture*)disp_drv->draw_buf->buf1;
		SDL_SetRenderTarget(renderer, NULL);

		// Background
		SDL_SetRenderDrawColor(renderer, 	0, 0, 0, 0);
		SDL_RenderClear(renderer);

		// Render
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		SDL_RenderSetClipRect(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_SetRenderTarget(renderer, texture);
	}
	lv_disp_flush_ready(disp_drv);
}

void GuiCLVGL::Update()
{
	lv_task_handler();
	ProcessEvents();
}

void GuiCLVGL::ProcessEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				mouse_event = true;
				if ((e.type==SDL_MOUSEBUTTONDOWN || e.type==SDL_MOUSEBUTTONUP) && e.button.button==SDL_BUTTON_LEFT) {
					mouse_event = true;
					mouse_pressed = (e.type==SDL_MOUSEBUTTONDOWN);
				}
				break;
			case SDL_QUIT:
				quit_requested = true;
				break;
			default:
				break;
		}
	}

}
#endif

#ifndef CLION
void GuiCLVGL::DisplayFlush(lv_disp_drv_t* pDriver, const lv_area_t* pArea, lv_color_t* pBuffer)
{
	assert(s_pThis!=0);

	assert(pArea != 0);
	int32_t x1 = pArea->x1;
	int32_t x2 = pArea->x2;
	int32_t y1 = pArea->y1;
	int32_t y2 = pArea->y2;

	assert(x1 <= x2);
	assert(y1 <= y2);
	assert(pBuffer != 0);

	assert(s_pThis->m_pFrameBuffer != 0);
	void *pDestination = (void *) (uintptr)(s_pThis->m_pFrameBuffer->GetBuffer() +
											y1 * s_pThis->m_pFrameBuffer->GetPitch() + x1 * LV_COLOR_DEPTH / 8);

	size_t nBlockLength = (x2 - x1 + 1) * LV_COLOR_DEPTH / 8;

	s_pThis->m_DMAChannel.SetupMemCopy2D(pDestination, pBuffer, nBlockLength, y2 - y1 + 1,
										 s_pThis->m_pFrameBuffer->GetPitch() - nBlockLength);

	assert(pDriver != 0);
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

	pData->state = s_pThis->m_PointerData.state;
	pData->point.x = s_pThis->m_PointerData.point.x;
	pData->point.y = s_pThis->m_PointerData.point.y;
}

void GuiCLVGL::MouseEventHandler(TMouseEvent Event, unsigned nButtons, unsigned nPosX, unsigned nPosY, int nWheelMove) {
	assert(s_pThis != 0);

	switch (Event) {
		case MouseEventMouseMove:
			s_pThis->m_PointerData.state = LV_INDEV_STATE_PR;
			s_pThis->m_PointerData.point.x = nPosX;
			s_pThis->m_PointerData.point.y = nPosY;
			break;
		case MouseEventMouseDown:
			if (nButtons & MOUSE_BUTTON_LEFT) {
				s_pThis->m_PointerData.state = LV_INDEV_STATE_PR;
				s_pThis->m_PointerData.point.x = nPosX;
				s_pThis->m_PointerData.point.y = nPosY;
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

void GuiCLVGL::MouseRemovedHandler(CDevice *pDevice, void *pContext) {
	assert(s_pThis != 0);
	s_pThis->m_pMouseDevice = 0;
}
#endif

void GuiCLVGL::LogPrint(const char* pMessage)
{
#ifndef CLION
	assert(pMessage != 0);
	size_t nLen = strlen(pMessage);
	assert(nLen > 0);

	char Buffer[nLen + 1];
	strcpy(Buffer, pMessage);

	if (Buffer[nLen - 1] == '\n') {
		Buffer[nLen - 1] = '\0';
	}

	CLogger::Get()->Write("lvgl", LogDebug, Buffer);
#else
	printf("LVGL: %s\n", pMessage);
#endif
}
