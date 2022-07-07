#pragma once
#include "../../../OS/OS.h"
#include "canvas/Canvas.h"
#include "window/Window.h"
#include "lvgl/lvgl.h"
#include "style/Style.h"
#include "../../DARICWindow.h"
#include <map>
#include <memory>

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
