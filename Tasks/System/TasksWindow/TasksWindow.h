#pragma once
#include "../WindowManager/WindowManager.h"

class TasksWindow : public OSDTask
{
  public:
    TasksWindow(int x, int y, int w, int h);
    ~TasksWindow();

    void Run();
    void UpdateGUI();

  private:
    const int HISTORY_SIZE = 64;
    std::list<size_t> used_history;
    lv_coord_t row_dsc[256];
    lv_coord_t col_dsc[7] = {lv_pct(31), lv_pct(10), lv_pct(15),           lv_pct(7),
                             lv_pct(7),  lv_pct(30), LV_GRID_TEMPLATE_LAST};

    void Maximise();
};
