#pragma once
#include <OS.h>
#include <Compiler.h>
#include <WindowManager/WindowManager.h>

class Editor : public OSDTask
{
  public:
    Editor(int x, int y, int w, int h);
    ~Editor();

    void Run();
    void LoadSourceCode(std::string volume, std::string directory, std::string filename);
    void Render();
    void RunWindowed();
    void FullscreenRun();
    void Debug();
    void UpdateDebugWindow();
    void Clear();
    void HandleException(DARICException &ex);

    bool debug_output = false;
    lv_obj_t *grid;
    lv_coord_t col_dsc[3] = {LV_PCT(100), 0, LV_GRID_TEMPLATE_LAST};
    lv_coord_t row_dsc[4] = {46, LV_PCT(100), 150, LV_GRID_TEMPLATE_LAST};

  private:
    std::string volume;
    std::string directory;
    std::string filename = "<No File>";
    TextEdit *edit = NULL;
    std::string loaded_code;
    lv_obj_t *ta1;
    lv_obj_t *ta2;
    lv_obj_t *ta3;
    lv_obj_t *ta4;
    lv_obj_t *status_text;
    lv_obj_t *status;
    lv_obj_t *buttons;
    DARICWindow *app = NULL;

    void Maximise();
    void CalculateLongestLine();
    static void DebugSelectHandler(lv_event_t *e);
    static void ResizeEventHandler(lv_event_t *e);
    static void RunHandler(lv_event_t *e);
    static void RunFullScreenHandler(lv_event_t *e);
    static void BuildHandler(lv_event_t *e);
    static void NewHandler(lv_event_t *e);
    static void LoadHandler(lv_event_t *e);
    static void SaveHandler(lv_event_t *e);
    static void ScrollBeginEvent(lv_event_t *e);
};
