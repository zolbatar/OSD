#include "WindowManager.h"
#include <capstone/capstone.h>
#include <capstone/platform.h>

#ifndef CLION
#include <circle/interrupt.h>
#include <circle/logger.h>
#include <circle/usb/usbhcidevice.h>
extern CScreenDevice* screen;
extern CInterruptSystem *interrupt;
extern CUSBHCIDevice *USBHCI;
#include "mouse.h"
#endif
#include "../../OS/OS.h"
#include "../TasksWindow.h"
#include "../Editor/Editor.h"
#include "../../GUI/Window/LVGLWindow.h"

extern "C"
{
#include "../../Lightning/lightning.h"
#include "../../Lightning/jit_private.h"
}

WindowManager::WindowManager()
{
	this->id = "@";
	this->name = "@";
	this->priority = TaskPriority::NoPreempt;

	// Init JIT
	init_jit("OS/D");
	jit_set_memory_functions(malloc, realloc, free);

	// Setup capstone
	cs_opt_mem setup;
	setup.malloc = malloc;
	setup.calloc = calloc;
	setup.realloc = realloc;
	setup.free = free;
	setup.vsnprintf = vsnprintf;
	cs_err err = cs_option(0, CS_OPT_MEM, (size_t)&setup);
	if (err!=CS_ERR_OK) {
		printf("Error (cs_option): %d\n", err);
	}
}

WindowManager::~WindowManager()
{
	DELETE clvgl;
}

void WindowManager::Run()
{
	SetNameAndAddToList();

	// Setup clvgl
	clvgl = NEW GuiCLVGL(screen, interrupt);
	clvgl->Initialize();
	SetupLVGLStyles();

#ifndef CLION
	// Mouse cursor
	/*	static lv_img_dsc_t mouse_cursor_icon;
		mouse_cursor_icon.header.always_zero = 0;
		mouse_cursor_icon.header.w = 14;
		mouse_cursor_icon.header.h = 20;
		mouse_cursor_icon.data_size = 280 * LV_IMG_PX_SIZE_ALPHA_BYTE;
		mouse_cursor_icon.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
		mouse_cursor_icon.data = mouse_cursor_icon_map;

		//	LV_IMG_DECLARE(mouse_cursor_icon);
		lv_obj_t *cursor = lv_img_create(lv_scr_act());
		lv_img_set_src(cursor, &mouse_cursor_icon);
		lv_indev_set_cursor(clvgl->GetMouse(), cursor);*/

	USBHCI->UpdatePlugAndPlay();
#endif

// Add right click menu to desktop
//	lv_obj_add_event_cb(lv_scr_act(), ClickEventHandler, LV_EVENT_LONG_PRESSED, this);

	DesktopStartup();

	while (!clvgl->QuitRequested()) {
#ifndef CLION
		clvgl->Update(USBHCI->UpdatePlugAndPlay());
#else
		clvgl->Update();
#endif

		//CLogger::Get()->Write("Window Manager", LogDebug, "Yielding");
		Yield();
	}

	// Must shut all other tasks down
#ifdef CLION
	for (auto& t: tasks) {
		if (t.first!="@") {
			t.second->TerminateTask();
		}
	}
#endif
}

void WindowManager::ReceiveDirectEx(DirectMessage* message)
{
	OSDTask* source = (OSDTask*)message->source;
	auto w = (Window*)source->GetWindow();
	auto c = w->GetCanvas();
	assert(w!=NULL);
	switch (message->type) {
		case Messages::WM_OpenWindow: {
			auto m = (WM_OpenWindow*)message->data;
			Window* w = new Window(source, m->canvas, m->fixed, m->title, m->x, m->y, m->width, m->height, m->canvas_w, m->canvas_h);
			Window::windows.insert(std::make_pair(m->id, w));
			source->SetWindow(w);
			break;
		}
		case Messages::WM_CloseWindow: {
			auto w = Window::windows.find(source->GetWindowID());
			if (w!=Window::windows.end()) {
				DELETE w->second;
				Window::windows.erase(w);
				source->SetWindow(NULL);
			}
			break;
		}
		case Messages::Canvas_PlotPixel: {
			auto m = (Coord1*)message->data;
			c->PlotPixel(m->x, m->y);
			break;
		}
		case Messages::Canvas_Clear: {
			c->Clear();
			break;
		}
		case Messages::Canvas_ClipOff: {
			c->ClipOff();
			break;
		}
		case Messages::Canvas_ClipOn: {
			auto m = (Coord2*)message->data;
			c->ClipOn(m->x1, m->y1, m->x2, m->y2);
			break;
		}
		case Messages::Canvas_DrawLine: {
			auto m = (Coord2W*)message->data;
			c->DrawLine(m->x1, m->y1, m->x2, m->y2, m->w);
			break;
		}
		case Messages::Canvas_Rectangle: {
			auto m = (Coord2W*)message->data;
			c->DrawRectangle(m->x1, m->y1, m->x2, m->y2, m->w);
			break;
		}
		case Messages::Canvas_RectangleFilled: {
			auto m = (Coord2W*)message->data;
			c->DrawRectangleFilled(m->x1, m->y1, m->x2, m->y2, m->w);
			break;
		}
		case Messages::Canvas_Triangle: {
			auto m = (Coord3W*)message->data;
			c->DrawTriangle(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
			break;
		}
		case Messages::Canvas_TriangleFilled: {
			auto m = (Coord3W*)message->data;
			c->DrawTriangleFilled(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
			break;
		}
		case Messages::Canvas_Circle: {
			auto m = (Coord1RW*)message->data;
			c->DrawCircle(m->x, m->y, m->r, m->w);
			break;
		}
		case Messages::Canvas_CircleFilled: {
			auto m = (Coord1RW*)message->data;
			c->DrawCircleFilled(m->x, m->y, m->r, m->w);
			break;
		}
		case Messages::Canvas_SetForegroundColour: {
			auto m = (Colour*)message->data;
			c->SetFG(m->colour);
			break;
		}
		case Messages::Canvas_SetBackgroundColour: {
			auto m = (Colour*)message->data;
			c->SetBG(m->colour);
			break;
		}
		case Messages::Canvas_Text: {
			auto m = (Coord1S*)message->data;
			c->DrawText(m->x, m->y, source->GetString(m->s));
			break;
		}
		case Messages::Canvas_TextCentre: {
			auto m = (Coord1S*)message->data;
			c->DrawTextCentre(m->x, m->y, source->GetString(m->s));
			break;
		}
		case Messages::Canvas_TextRight: {
			auto m = (Coord1S*)message->data;
			c->DrawTextRight(m->x, m->y, source->GetString(m->s));
			break;
		}
		case Messages::Canvas_SetFont: {
			auto m = (SetFont*)message->data;
			c->SetFont(source->GetString(m->ff), source->GetString(m->fs), m->size);
			break;
		}
		case Messages::Canvas_PrintString: {
			c->PrintString((const char*)message->data);
			break;
		}
		case Messages::Canvas_PrintNewLine: {
			c->PrintNewLine();
			break;
		}
		case Messages::Canvas_PrintTab: {
			auto n = (int64_t*)message->data;
			c->PrintTab(*n);
			break;
		}
		case Messages::Canvas_Enable_Shadow: {
			c->EnableDoubleBuffering();
			break;
		}
		case Messages::Canvas_Flip: {
			c->Flip();
			break;
		}
		default:
#ifndef CLION
			CLogger::Get()->Write("GUI", LogDebug, "Unknown directex message received %d %p", message->type, message->source);
#else
			printf("Unknown directex message received %d %p", message->type, message->source);
#endif
			assert(0);
	}
#ifndef CLION
	//	clvgl->Update(USBHCI->UpdatePlugAndPlay());
#else
	clvgl->Update();
#endif
}

void WindowManager::DesktopStartup()
{
#ifndef CLION

	/*	auto clock3 = NEW DARICWindow("Clock3", false, 200, 450, 700, 600);
		clock3->LoadSourceCode(":SD.$.Welcome.Clock3");
		clock3->Start();*/

	/*		auto graphics2d = NEW DARICWindow("Graphics 2D", false, 20, 450, 600, 600);
		graphics2d->LoadSourceCode(":SD.$.Welcome.Graphics2d");
		graphics2d->Start();

		auto sierpinski = NEW DARICWindow("Sierpinski", false, 50, 50, 500, 500);
		sierpinski->LoadSourceCode(":SD.$.Welcome.Sierpinski");
		sierpinski->Start();
*/
		auto fonts = NEW DARICWindow("Fonts", false, 100, 20, 400, 400, 1600, 900);
		fonts->LoadSourceCode(":SD.$.Welcome.Fonts");
		fonts->Start();

//		auto mandelbrot = NEW DARICWindow("Mandelbrot", false, 700, 500, 400, 400);
//		mandelbrot->LoadSourceCode(":SD.$.Welcome.Mandelbrot");
//		mandelbrot->Start();
/*
		auto tester = NEW DARICWindow("Tester", false, 1250, 100, 500, 700);
		tester->LoadSourceCode(":SD.$.Welcome.Tester");
		tester->Start();

		auto raytracer = NEW DARICWindow("Ray Tracer", false, 650, 700, 640, 350);
		raytracer->LoadSourceCode(":SD.$.Welcome.Raytracer");
		raytracer->Start();*/

		auto tasks = NEW TasksWindow(1100, 600, 750, 250);
		tasks->Start();

//		auto clock = NEW DARICWindow("Clock", false, 800, 100, 400, 300);
//		clock->LoadSourceCode(":SD.$.Welcome.Clock");
//		clock->Start();
#endif
}

void WindowManager::CreateMenu(int x, int y, OSDTask* task, std::string title, Menu* menu)
{
	this->menu = menu;

	menu_win = lv_mywin_create(lv_scr_act(), MENU_HEADER_HEIGHT);
	lv_obj_set_x(menu_win, x);
	lv_obj_set_y(menu_win, y);
	lv_obj_set_width(menu_win, 160);
	lv_obj_set_height(menu_win, 500);
	lv_obj_add_style(menu_win, &style_menu, LV_STATE_DEFAULT);

	auto content = lv_mywin_get_content(menu_win);
	lv_obj_add_style(content, &style_window_content, LV_STATE_DEFAULT);

	// Header
	lv_mywin_add_title(menu_win, title.c_str());
	auto header = lv_mywin_get_header(menu_win);
	lv_obj_add_style(header, &style_window_header, LV_STATE_DEFAULT);
	lv_obj_add_style(header, &style_window_header_inactive, LV_STATE_DEFAULT);

	// Vertical container
	auto cont_col = lv_obj_create(lv_mywin_get_content(menu_win));
	lv_obj_set_width(cont_col, lv_pct(100));
	lv_obj_set_height(cont_col, lv_pct(100));
	lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(cont_col, &style_menu_container, LV_STATE_DEFAULT);

	// Items
	for (auto& mi : menu->items) {
		switch (mi.type) {
			case MenuItemType::Item: {
				auto label = lv_label_create(cont_col);
				lv_label_set_text(label, mi.v.c_str());
				lv_obj_add_style(label, &style_menu_item, LV_STATE_DEFAULT);
				break;
			}
			default:
				break;
		}
	}
}

void WindowManager::ClickEventHandler(lv_event_t* e)
{
	lv_point_t p;
	lv_indev_t* indev = lv_indev_get_act();
	lv_indev_type_t indev_type = lv_indev_get_type(indev);
	if (indev_type==LV_INDEV_TYPE_POINTER) {
		lv_indev_get_point(indev, &p);

		// Create menu window
		auto t = (WindowManager*)e->user_data;
		auto menu = new Menu();

		// Applications
		MenuItem mi;
		mi.type = MenuItemType::Item;
		mi.v = "Mandelbrot";
		menu->items.push_back(std::move(mi));
		MenuItem mi2;
		mi2.type = MenuItemType::Item;
		mi2.v = "Tester";
		menu->items.push_back(std::move(mi2));
		MenuItem mi3;
		mi3.type = MenuItemType::Item;
		mi3.v = "Clock";
		menu->items.push_back(std::move(mi3));

		t->CreateMenu(p.x, p.y, NULL, "OS/D", menu);
	}
}

