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

extern int dm;

extern "C"
{
#include "../../Lightning/lightning.h"
#include "../../Lightning/jit_private.h"
}

WindowManager::WindowManager()
{
	this->id = "@";
	this->name = "@";
	message_queue_size = MAX_MESSAGE_QUEUE;
	message_queue = NEW Message[message_queue_size];

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

	// Setup clvgl
#ifdef CLION
	clvgl = NEW GuiCLVGL();
#else
	clvgl = NEW GuiCLVGL(screen, interrupt);
#endif
	clvgl->Initialize();
#ifndef CLION
	CLogger::Get()->Write("FontManager", LogDebug, "Setting up styles");
#endif
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
}

WindowManager::~WindowManager()
{
	DELETE clvgl;
}

void WindowManager::Run()
{
	SetNameAndAddToList();

	// Add right click menu to desktop
	lv_obj_add_event_cb(lv_scr_act(), ClickEventHandler, LV_EVENT_LONG_PRESSED, this);

	// Startup applications
	DesktopStartup();

	while (!clvgl->QuitRequested()) {
#ifndef CLION
		clvgl->Update(USBHCI->UpdatePlugAndPlay());
		Yield();
#else
		LockVLGL("Refresh");
		clvgl->Update();
		UnlockVLGL();
#endif

		// Process messages
		for (size_t i = 0; i<message_queue_position; i++) {
			Message* message = &message_queue[i];
			OSDTask* source = (OSDTask*)message->source;

			// Use task override, so allocations are allocated to the right task
			task_override = source;
			switch (message->type) {

				// Window manager
				case Messages::WM_OpenWindow: {
					auto m = (WM_OpenWindow*)&message->data;
					Window* w = new Window(source, m->canvas, m->fixed, m->title, m->x, m->y, m->width, m->height);
					Window::windows.insert(std::make_pair(m->id, w));
					source->SetWindow(m->id, w);
					break;
				}
				case Messages::WM_CloseWindow: {
					auto w = Window::windows.find(source->GetWindowID());
					if (w!=Window::windows.end()) {
						DELETE w->second;
						Window::windows.erase(w);
						source->SetWindow(source->GetWindowID(), NULL);
					}
					break;
				}

					// Canvas
				case Messages::Canvas_Clear: {
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->Clear();
					break;
				}
				case Messages::Canvas_DrawLine: {
					auto m = (Coord2W*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawLine(m->x1, m->y1, m->x2, m->y2, m->w);
					break;
				}
				case Messages::Canvas_Rectangle: {
					auto m = (Coord2W*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawRectangle(m->x1, m->y1, m->x2, m->y2, m->w);
					break;
				}
				case Messages::Canvas_RectangleFilled: {
					auto m = (Coord2W*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawRectangleFilled(m->x1, m->y1, m->x2, m->y2, m->w);
					break;
				}
				case Messages::Canvas_Triangle: {
					auto m = (Coord3W*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawTriangle(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
					break;
				}
				case Messages::Canvas_TriangleFilled: {
					auto m = (Coord3W*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawTriangleFilled(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
					break;
				}
				case Messages::Canvas_Circle: {
					auto m = (Coord1RW*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawCircle(m->x, m->y, m->r, m->w);
					break;
				}
				case Messages::Canvas_CircleFilled: {
					auto m = (Coord1RW*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->DrawCircleFilled(m->x, m->y, m->r, m->w);
					break;
				}
				case Messages::Canvas_PlotPixel: {
					auto m = (Coord1*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->PlotPixel(m->x, m->y);
					break;
				}
				case Messages::Canvas_SetForegroundColour: {
					auto m = (Colour*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->SetFG(m->colour);
					break;
				}
				case Messages::Canvas_SetBackgroundColour: {
					auto m = (Colour*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->SetBG(m->colour);
					break;
				}
				case Messages::Canvas_PrintString: {
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->PrintString((const char*)&message->data);
					break;
				}
				case Messages::Canvas_PrintNewLine: {
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->PrintNewLine();
					break;
				}
				case Messages::Canvas_PrintStringLong: {
					auto m = (Address*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->PrintString((const char*)m->address);
					break;
				}
				case Messages::Canvas_PrintTab: {
					auto m = (Integer*)&message->data;
					auto w = (Window*)source->GetWindow();
					assert(w!=NULL);
					w->GetCanvas()->PrintTab(m->v);
					break;
				}
				default:
#ifndef CLION
					CLogger::Get()->Write("GUI", LogDebug, "Unknown message received %d %p", message->type, message->source);
#else
					printf("Unknown message received %d %p", message->type, message->source);
#endif
					assert(0);
			}
			task_override = NULL;
		}
		message_queue_position = 0;
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

void WindowManager::DesktopStartup()
{
#ifndef CLION
	auto mandelbrot = NEW DARICWindow("Mandelbrot", false, 100*dm, 100*dm, 400*dm, 400*dm);
	mandelbrot->LoadSourceCode(":SD.$.Welcome.Mandelbrot");
	mandelbrot->Start();

	auto tester = NEW DARICWindow("Tester", false, 1250*dm, 100*dm, 500*dm, 700*dm);
	tester->LoadSourceCode(":SD.$.Welcome.Tester");
	tester->Start();

	auto clock = NEW DARICWindow("Clock", false, 800*dm, 100*dm, 400*dm, 300*dm);
	clock->LoadSourceCode(":SD.$.Welcome.Clock");
	clock->Start();

	auto tasks = NEW TasksWindow(1200*dm, 600*dm, 600*dm, 400*dm);
	tasks->Start();

/*	auto editor = NEW Editor(200*dm, 450*dm, 700*dm, 600*dm);
	editor->LoadSourceCode(":SD.$.Welcome.Mandelbrot");
	editor->Start();*/

/*		auto clock3 = NEW DARICWindow("Clock3", false, 200*dm, 450*dm, 700*dm, 600*dm);
	clock3->LoadSourceCode(":SD.$.Welcome.Clock3");
	clock3->Start();*/

	auto graphics2d = NEW DARICWindow("Graphics 2D", false, 200*dm, 450*dm, 600*dm, 600*dm);
	graphics2d->LoadSourceCode(":SD.$.Welcome.Graphics2d");
	graphics2d->Start();
#else
	auto tasks = NEW TasksWindow(1200*dm, 200*dm, 550*dm, 400*dm);
	std::thread t1(&DARICWindow::Start, tasks);
	t1.detach();

/*	auto editor = NEW Editor(200*dm, 450*dm, 700*dm, 600*dm);
	editor->LoadSourceCode(":SD.$.Welcome.Tester");
	std::thread t2(&Editor::Start, editor);
	t2.detach();*/

/*	auto clock = NEW DARICWindow("Clock", false, 1000*dm, 100*dm, 400*dm, 300*dm);
	clock->LoadSourceCode(":SD.$.Welcome.Clock");
	std::thread t2(&DARICWindow::Start, clock);
	t2.detach();*/

	/*    auto mandelbrot = NEW DARICWindow("Mandelbrot", false, 100*dm, 600*dm, 400*dm, 400*dm);
		mandelbrot->LoadSourceCode(":SD.$.Welcome.Mandelbrot");
		std::thread t3(&DARICWindow::Start, mandelbrot);
		t3.detach();*/

/*		auto tester = NEW DARICWindow("Tester", false, 100*dm, 600*dm, 400*dm, 400*dm);
		tester->LoadSourceCode(":SD.$.Welcome.Tester");
		std::thread t4(&DARICWindow::Start, tester);
		t4.detach();*/

/*		auto clock3 = NEW DARICWindow("Clock3", false, 100*dm, 600*dm, 400*dm, 400*dm);
		clock3->LoadSourceCode(":SD.$.Welcome.Clock3");
		std::thread t4(&DARICWindow::Start, clock3);
		t4.detach();*/

	auto graphics2d = NEW DARICWindow("Graphics 2D", false, 100*dm, 600*dm, 400*dm, 400*dm);
	graphics2d->LoadSourceCode(":SD.$.Welcome.Graphics2D");
	std::thread t4(&DARICWindow::Start, graphics2d);
	t4.detach();

#endif
}

void WindowManager::CreateMenu(int x, int y, OSDTask* task, std::string title, Menu* menu)
{
	this->menu = menu;

	menu_win = lv_win_create(lv_scr_act(), MENU_HEADER_HEIGHT);
	lv_obj_set_x(menu_win, x);
	lv_obj_set_y(menu_win, y);
	lv_obj_set_width(menu_win, 160);
	lv_obj_set_height(menu_win, 500);
	lv_obj_add_style(menu_win, &style_menu, LV_STATE_DEFAULT);

	auto content = lv_win_get_content(menu_win);
	lv_obj_add_style(content, &style_window_content, LV_STATE_DEFAULT);

	// Header
	lv_win_add_title(menu_win, title.c_str());
	auto header = lv_win_get_header(menu_win);
	lv_obj_add_style(header, &style_window_header, LV_STATE_DEFAULT);
	lv_obj_add_style(header, &style_window_header_inactive, LV_STATE_DEFAULT);

	// Vertical container
	auto cont_col = lv_obj_create(lv_win_get_content(menu_win));
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


