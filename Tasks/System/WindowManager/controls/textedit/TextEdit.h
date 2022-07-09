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
    void SetMaximisedMode();
    std::string GetText();
    void Backspace();

    int screen_x = 0;
    int screen_y = 0;
    int x = 0;
    int y = 0;
    int canvas_w;
    int canvas_h;

  private:
    bool show_blocks = true;
    bool maximised_mode = false;
    uint32_t fg = 0x404040;
    uint32_t bg = 0xFFFFFF;
    uint32_t bg_char = 0xF0F0F0;
    uint32_t cursor = 0x0;
    void SetupCanvas();
    void Render();
    void CalculateLongestLine();
    Canvas *canvas = NULL;
    lv_obj_t *parent_obj;
    lv_obj_t *obj;
    Mode mode = Mode::Insert;
    size_t longest_line = 0;
    std::vector<std::string> code;
    void Insert(char c);

    static void ScrollEventHandler(lv_event_t *e);
};
