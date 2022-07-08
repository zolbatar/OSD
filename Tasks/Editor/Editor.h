#pragma once
#include <OS.h>
#include <Compiler.h>
#include <WindowManager/WindowManager.h>

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
    void SetupCanvas();

    int screen_x = 0;
    int screen_y = 0;
    int x = 0;
    int y = 0;
    int canvas_w;
    int canvas_h;

  private:
    std::string volume;
    std::string directory;
    std::string filename;
    Mode mode = Mode::Insert;
    size_t longest_line = 0;
    std::vector<std::string> code;
    lv_obj_t *obj = NULL;
    lv_obj_t *obj_parent = NULL;
    lv_obj_t *ta1;
    lv_obj_t *ta2;
    lv_obj_t *ta3;
    lv_obj_t *ta4;
    lv_obj_t *status_text;
    lv_coord_t col_dsc[3] = {LV_PCT(100), 400, LV_GRID_TEMPLATE_LAST};
    lv_coord_t row_dsc[4] = {46, LV_PCT(100), 150, LV_GRID_TEMPLATE_LAST};

    void Maximise();
    void CalculateLongestLine();
    static void ScrollEventHandler(lv_event_t *e);
    static void ResizeEventHandler(lv_event_t *e);
    static void RunHandler(lv_event_t *e);
    static void RunFullScreenHandler(lv_event_t *e);
    static void BuildHandler(lv_event_t *e);
    static void ScrollBeginEvent(lv_event_t *e);
};
