#pragma once
#include <OS.h>
#include "lvgl/lvgl.h"
#include "controls/Control.h"
#include "controls/canvas/Canvas.h"
#include "style/Style.h"
#include "window/Window.h"
#include "../Tasks/DARICWindow.h"

class FileType
{
  public:
    FileType(std::string extension, std::string icon, std::string application)
        : extension(extension), icon(icon), application(application)
    {
    }
    std::string extension;
    std::string icon;
    std::string application;
};

struct Icon
{
    std::string filename;
    lv_img_dsc_t *image = NULL;
};

class WindowManager : public OSDTask
{
  public:
    WindowManager();
    ~WindowManager();
    void Run();
    void ReceiveDirectEx(DirectMessage *message);
    static lv_img_dsc_t *GetIcon(std::string name);
    static void LoadIcon(std::string filename, std::string name);
    static FileType *GetFileType(std::string type);

  private:
    GuiCLVGL *clvgl;
    static std::map<std::string, Icon> icons;
    static std::map<std::string, FileType> types;
    lv_img_dsc_t *mouse_cursor;

    static lv_style_t *CreateStyle();
    void SetupLVGLStyles();
    static lv_img_dsc_t *LoadPNG(std::string filename, int w, int h);
    static void ClickEventHandler(lv_event_t *e);
};
