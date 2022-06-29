#pragma once
#include <string>
#include "../Controls/Control.h"
#include "../Controls/Canvas.h"
#include "../../Tasks/System/WindowManager/Style.h"

class Window
{
  public:
    Window(OSDTask *task, bool pure_canvas, bool fixed, std::string title, int x, int y, int w, int h, int canvas_w,
           int canvas_h);
    ~Window();
    lv_obj_t *GetLVGLWindow()
    {
        return win;
    }
    Canvas *GetCanvas()
    {
        return canvas;
    }
    void DeleteCanvas();
    void CreateCanvas(int canvas_w, int canvas_h);
    int64_t GetContentWidth()
    {
        return canvas->GetContentWidth();
    }
    int64_t GetContentHeight()
    {
        return canvas->GetContentHeight();
    }
    void SetActive();
    void SetInactive();
    bool GetActive()
    {
        return active;
    }
    OSDTask *GetTask()
    {
        return task;
    }
    void Maximise(bool full_maximise);

    static std::map<std::string, Window *> windows;

  private:
    static void MinimiseClicked(_lv_event_t *e);
    static void MaximiseClicked(_lv_event_t *e);
    static void CloseClicked(_lv_event_t *e);
    static void DragEventHandler(lv_event_t *e);
    static void ClickEventHandler(lv_event_t *e);
    OSDTask *task;
    std::string title;
    int x1;
    int y1;
    int x2;
    int y2;
    int width;
    int height;
    int z = 0;
    bool active = false;
    lv_obj_t *win;
    lv_obj_t *header;
    Canvas *canvas = NULL;
    std::list<Control> children;
    lv_group_t *group;
    bool maximised = false;
};
