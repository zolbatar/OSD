#pragma once
#include <OS.h>
#include <WindowManager/WindowManager.h>

enum class Mode
{
    Insert,
    Overwrite
};

class TextEdit : public Control
{
  public:
    TextEdit(OSDTask *task, lv_obj_t *parent);
    ~TextEdit();
    lv_obj_t *GetObjectParent()
    {
        return parent_obj;
    }
    lv_obj_t *GetObject()
    {
        return obj;
    }
    void SetText(std::string text);
    void Resized();
    void HandleKey(Key k);
    std::string GetText();

    int screen_x = 0;
    int screen_y = 0;
    int x = 0;
    int y = 0;
    int canvas_w;
    int canvas_h;

  private:
    void SetupCanvas();
    void Render();
    void CalculateLongestLine();
    Canvas *canvas = NULL;
    lv_obj_t *parent_obj;
    lv_obj_t *obj;
    Mode mode = Mode::Insert;
    size_t longest_line = 0;
    std::vector<std::string> code;

    static void ScrollEventHandler(lv_event_t *e);
};
