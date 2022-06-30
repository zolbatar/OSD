#pragma once
#include <vector>
#include "../System/WindowManager/WindowManager.h"

enum class Mode
{
    Insert,
    Overwrite
};

class Editor : public OSDTask
{
  public:
    Editor(int x, int y, int w, int h);
    ~Editor();

    void Run();
    void LoadSourceCode(std::string volume, std::string directory, std::string filename);
    void Render();
    lv_obj_t *GetObject()
    {
        return obj;
    }
    void RunWindowed();
    void FullscreenRun();
    void Debug();
    void UpdateDebugWindow();

    int screen_x = 0;
    int screen_y = 0;
    int x = 0;
    int y = 0;
    int canvas_w;
    int canvas_h;
    lv_obj_t *GetButtons()
    {
        return buttons;
    }

  private:
    std::string volume;
    std::string directory;
    std::string filename;
    Mode mode = Mode::Insert;
    size_t longest_line = 0;
    std::vector<std::string> code;
    lv_obj_t *obj;
    lv_obj_t *buttons;
    lv_obj_t *debug = NULL;
    lv_obj_t *tab_tokens;
    lv_obj_t *tab_parser;
    lv_obj_t *tab_ir;
    lv_obj_t *tab_native;
    lv_obj_t *ta1;
    lv_obj_t *ta2;
    lv_obj_t *ta3;
    lv_obj_t *ta4;

    void CalculateLongestLine();
    static void ScrollEventHandler(lv_event_t *e);
    static void MoveEventHandler(lv_event_t *e);
    static void ResizeEventHandler(lv_event_t *e);
    static void ContextMenuEventHandler(lv_event_t *e);
    static void RunHandler(lv_event_t *e);
    static void RunFullScreenHandler(lv_event_t *e);
    static void BuildHandler(lv_event_t *e);
};
