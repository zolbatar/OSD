#include "WindowManager.h"
#include <capstone/capstone.h>
#include <capstone/platform.h>
#include <circle/interrupt.h>
#include <circle/logger.h>
#include <circle/usb/usbhcidevice.h>
extern CScreenDevice *screen;
extern CInterruptSystem *interrupt;
extern CUSBHCIDevice *USBHCI;
#include "mouse.h"
#include "../../OS/OS.h"
#include "../TasksWindow/TasksWindow.h"
#include "../../Editor/Editor.h"
#include "../../GUI/Window/LVGLWindow.h"
std::map<std::string, Icon> WindowManager::icons;
std::map<std::string, FileType> WindowManager::types;

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

    // Setup capstone
    cs_opt_mem setup;
    setup.malloc = malloc;
    setup.calloc = calloc;
    setup.realloc = realloc;
    setup.free = free;
    setup.vsnprintf = vsnprintf;
    cs_err err = cs_option(0, CS_OPT_MEM, (size_t)&setup);
    if (err != CS_ERR_OK)
    {
        CLogger::Get()->Write("Window Manager", LogPanic, "Error (cs_option): %d\n", err);
    }
}

WindowManager::~WindowManager()
{
    delete clvgl;
}

void WindowManager::Run()
{
    SetNameAndAddToList();

    // Setup clvgl
    clvgl = new GuiCLVGL(screen, interrupt);
    clvgl->Initialize();
    SetupLVGLStyles();

    // Setup filetypes
    types.insert(std::make_pair("DARIC", FileType("DARIC", "Sloth", "")));
    types.insert(std::make_pair("PNG", FileType("PNG", "Image", "")));
    types.insert(std::make_pair("TXT", FileType("TXT", "Text", "")));

    //	LV_IMG_DECLARE(mouse_cursor_icon);
    lv_obj_t *cursor = lv_img_create(lv_scr_act());
    lv_img_set_src(cursor, mouse_cursor);
    lv_indev_set_cursor(clvgl->GetMouse(), cursor);

    USBHCI->UpdatePlugAndPlay();

    // Add right click menu to desktop
    //	lv_obj_add_event_cb(lv_scr_act(), ClickEventHandler, LV_EVENT_LONG_PRESSED, this);

    while (!clvgl->QuitRequested())
    {
        clvgl->Update(USBHCI->UpdatePlugAndPlay());

        // CLogger::Get()->Write("Window Manager", LogDebug, "Yielding");
        Yield();
    }

    // Must shut all other tasks down
    /*	for (auto& t: tas) {
            if (t.first!="@") {
                t.second->TerminateTask();
            }
        }*/
}

void WindowManager::ReceiveDirectEx(DirectMessage *message)
{
    OSDTask *source = (OSDTask *)message->source;
    auto w = (Window *)source->GetWindow();
    auto c = w->GetCanvas();
    assert(w != NULL);
    switch (message->type)
    {
    case Messages::WM_OpenWindow: {
        auto m = (WM_OpenWindow *)message->data;
        Window *w = new Window(source, m->canvas, m->fixed, m->title, m->x, m->y, m->width, m->height, m->canvas_w,
                               m->canvas_h);
        Window::windows.insert(std::make_pair(m->id, w));
        source->SetWindow(w);
        break;
    }
    case Messages::WM_CloseWindow: {
        auto w = Window::windows.find(source->GetWindowID());
        if (w != Window::windows.end())
        {
            delete w->second;
            Window::windows.erase(w);
            source->SetWindow(NULL);
        }
        break;
    }
    case Messages::Canvas_Mode: {
        auto m = (Coord1 *)message->data;
        c->Mode(m->x, m->y);
        break;
    }
    case Messages::Canvas_PlotPixel: {
        auto m = (Coord1 *)message->data;
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
        auto m = (Coord2 *)message->data;
        c->ClipOn(m->x1, m->y1, m->x2, m->y2);
        break;
    }
    case Messages::Canvas_DrawLine: {
        auto m = (Coord2W *)message->data;
        c->DrawLine(m->x1, m->y1, m->x2, m->y2, m->w);
        break;
    }
    case Messages::Canvas_Rectangle: {
        auto m = (Coord2W *)message->data;
        c->DrawRectangle(m->x1, m->y1, m->x2, m->y2, m->w);
        break;
    }
    case Messages::Canvas_RectangleFilled: {
        auto m = (Coord2W *)message->data;
        c->DrawRectangleFilled(m->x1, m->y1, m->x2, m->y2, m->w);
        break;
    }
    case Messages::Canvas_Triangle: {
        auto m = (Coord3W *)message->data;
        c->DrawTriangle(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
        break;
    }
    case Messages::Canvas_TriangleFilled: {
        auto m = (Coord3W *)message->data;
        c->DrawTriangleFilled(m->x1, m->y1, m->x2, m->y2, m->x3, m->y3, m->w);
        break;
    }
    case Messages::Canvas_Circle: {
        auto m = (Coord1RW *)message->data;
        c->DrawCircle(m->x, m->y, m->r, m->w);
        break;
    }
    case Messages::Canvas_CircleFilled: {
        auto m = (Coord1RW *)message->data;
        c->DrawCircleFilled(m->x, m->y, m->r, m->w);
        break;
    }
    case Messages::Canvas_SetForegroundColour: {
        auto m = (Colour *)message->data;
        c->SetFG(m->colour);
        break;
    }
    case Messages::Canvas_SetBackgroundColour: {
        auto m = (Colour *)message->data;
        c->SetBG(m->colour);
        break;
    }
    case Messages::Canvas_Text: {
        auto m = (Coord1S *)message->data;
        c->DrawText(m->x, m->y, source->GetString(m->s));
        break;
    }
    case Messages::Canvas_TextCentre: {
        auto m = (Coord1S *)message->data;
        c->DrawTextCentre(m->x, m->y, source->GetString(m->s));
        break;
    }
    case Messages::Canvas_TextRight: {
        auto m = (Coord1S *)message->data;
        c->DrawTextRight(m->x, m->y, source->GetString(m->s));
        break;
    }
    case Messages::Canvas_SetFont: {
        auto m = (SetFont *)message->data;
        c->SetFont(source->GetString(m->ff), source->GetString(m->fs), m->size);
        break;
    }
    case Messages::Canvas_PrintString: {
        c->PrintString((const char *)message->data);
        break;
    }
    case Messages::Canvas_PrintNewLine: {
        c->PrintNewLine();
        break;
    }
    case Messages::Canvas_PrintTab: {
        auto n = (int64_t *)message->data;
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
        CLogger::Get()->Write("Window Manager", LogPanic, "Unknown directex message received %d %p", (int)message->type,
                              message->source);
    }
    //	clvgl->Update(USBHCI->UpdatePlugAndPlay());
}

void WindowManager::ClickEventHandler(lv_event_t *e)
{
    /*	lv_point_t p;
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
        }*/
}

lv_img_dsc_t *WindowManager::GetIcon(std::string name)
{
    auto f = icons.find(name);
    if (f == icons.end())
    {
        return NULL;
    }
    if (f->second.image == NULL)
    {
        f->second.image = LoadPNG(f->second.filename, 64, 64);
    }
    return f->second.image;
}

FileType *WindowManager::GetFileType(std::string type)
{
    auto f = types.find(type);
    if (f == types.end())
    {
        return NULL;
    }
    return &f->second;
}
