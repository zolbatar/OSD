#include "GUI.h"

#ifndef CLION
#include <circle/interrupt.h>
#include <circle/usb/usbhcidevice.h>
extern CScreenDevice* screen;
extern CInterruptSystem *interrupt;
extern CUSBHCIDevice *USBHCI;
#include "mouse.h"
#endif

GUI::GUI()
{
	// Setup clvgl
#ifdef CLION
	clvgl = NEW GuiCLVGL();
#else
	clvgl = NEW GuiCLVGL(screen, interrupt);
#endif
	clvgl->Initialize();
	SetupLVGLStyles();

#ifndef CLION
	// Mouse cursor
	static lv_img_dsc_t mouse_cursor_icon;
	mouse_cursor_icon.header.always_zero = 0;
	mouse_cursor_icon.header.w = 14;
	mouse_cursor_icon.header.h = 20;
	mouse_cursor_icon.data_size = 280 * LV_IMG_PX_SIZE_ALPHA_BYTE;
	mouse_cursor_icon.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
	mouse_cursor_icon.data = mouse_cursor_icon_map;

	//	LV_IMG_DECLARE(mouse_cursor_icon);
	lv_obj_t *cursor = lv_img_create(lv_scr_act());
	lv_img_set_src(cursor, &mouse_cursor_icon);
	lv_indev_set_cursor(clvgl->GetMouse(), cursor);

	USBHCI->UpdatePlugAndPlay();
#endif
}

GUI::~GUI()
{
	DELETE clvgl;
}

void GUI::Run()
{
	SetName("GUI");
	std::queue<Message> local_message_queue;
	while (!clvgl->QuitRequested()) {
#ifndef CLION
		clvgl->Update(USBHCI->UpdatePlugAndPlay());
#else
		clvgl->Update();
#endif
		Yield();

		// Process messages
		while (!message_queue.empty()) {
			auto message = message_queue.front();
			message_queue.pop();

			OSDTask* source = (OSDTask*)message.source;
			switch (message.type) {

				// Window manager
				case Messages::WM_OpenWindow: {
					auto m = (WM_OpenWindow*)message.data;
					Window* w = new Window(true, m->title, m->x, m->y, m->width, m->height);
					w->SetActive();

					// Set all other windows as inactive
					for (auto& w: windows)
						w.second->SetInactive();

					windows.insert(std::make_pair(m->id, w));
					source->SetWindow(m->id, w);
					delete m;
					break;
				}
				case Messages::WM_CloseWindow: {
					auto w = windows.find(source->GetWindowID());
					if (w!=windows.end()) {
						delete w->second;
						windows.erase(w);
						source->SetWindow(source->GetWindowID(), NULL);
					}
					break;
				}

					// Canvas
				case Messages::Canvas_Clear: {
					auto w = (Window*)source->GetWindow();
					if (w!=NULL)
						w->GetCanvas()->Clear();
					break;
				}
				case Messages::Canvas_DrawLine: {
					auto m = (Canvas_DrawLine*)message.data;
					auto w = (Window*)source->GetWindow();
					if (w!=NULL)
						w->GetCanvas()->DrawLine(m->x1, m->y1, m->x2, m->y2);
					delete m;
					break;
				}
				case Messages::Canvas_PlotPixel: {
					auto m = (Canvas_PlotPixel*)message.data;
					auto w = (Window*)source->GetWindow();
					if (w!=NULL)
						w->GetCanvas()->PlotPixel(m->x, m->y);
					delete m;
					break;
				}
				case Messages::Canvas_SetForegroundColour: {
					auto m = (Canvas_Colour*)message.data;
					auto w = (Window*)source->GetWindow();
					if (w!=NULL)
						w->GetCanvas()->SetFG(m->colour);
					delete m;
					break;
				}
			}
		}
	}

	// Must shut all other tasks down
#ifdef CLION
	for (auto& t: tasks) {
		if (t.first!="GUI") {
			t.second->Terminate();
		}
	}
#endif
}


