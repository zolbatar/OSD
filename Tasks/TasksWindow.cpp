#include "TasksWindow.h"

TasksWindow::TasksWindow(int x, int y, int w, int h) {
    this->d_x = x;
    this->d_y = y;
    this->d_w = w;
    this->d_h = h;
    this->id = std::to_string(task_id++);
    this->name = "Task Manager";
    SetNameAndAddToList();
}

void TasksWindow::Run() {
    // Create Window
    auto mess = SendGUIMessage();
    mess->type = Messages::WM_OpenWindow;
    mess->source = this;
    auto m = (WM_OpenWindow *) &mess->data;
    strcpy(m->id, "TASKS");
    strcpy(m->title, "Task Manager");
    m->x = d_x;
    m->y = d_y;
    m->width = d_w;
    m->height = d_h;
    m->canvas = false;
    m->fixed = false;

    // Wait for window to be created
    Window *w;
    do {
        Yield();
        w = (Window *) GetWindow();
    } while (w == NULL);

    // Fill out history with zeroes
    for (auto i = 0; i < HISTORY_SIZE; i++)
        used_history.push_back(0);

    // Do stuff
    while (1) {
        UpdateTasks();
        Sleep(1000);
    }

    TerminateTask();
}

void TasksWindow::UpdateTasks() {
    OSDTask::vlgl_mutex.lock();
    auto w = ((Window *) this->GetWindow())->GetLVGLWindow();
    lv_obj_clean(lv_win_get_content(w));

    auto m = CalculateMem();
    used_history.pop_front();
    used_history.push_back(m.used);

    // Vertical container
    auto cont_col = lv_obj_create(lv_win_get_content(w));
    lv_obj_set_width(cont_col, lv_pct(100));
    lv_obj_set_height(cont_col, lv_pct(100));
    lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(cont_col, &style_grid, LV_STATE_DEFAULT);

    // Memory chart
    auto chart = lv_chart_create(cont_col);
    lv_obj_set_flex_grow(chart, 1);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, HISTORY_SIZE);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 0, 0, 0, false, 0);
    lv_chart_set_div_line_count(chart, 0, 0);
    lv_obj_add_style(chart, &style_chart, LV_PART_MAIN);
    lv_obj_add_style(chart, &style_chart_bar, LV_PART_ITEMS);

    /*Add two data series*/
    lv_chart_series_t *ser1 = lv_chart_add_series(chart, CONTROL_HIGHLIGHT_COLOUR, LV_CHART_AXIS_PRIMARY_Y);

    // Set points
    auto m1 = 0;
    for (auto &mc: used_history) {
        auto m = static_cast<int>(mc);
        lv_chart_set_next_value(chart, ser1, m);
        if (m > m1) m1 = m;
    }
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, m1);

    // Container
#ifndef CLION
    const int sz = tasks_list.size()+3;
#else
    const int sz = tasks_list.size() + 1;
#endif
    static lv_coord_t row_dsc[256];
    for (int i = 0; i < sz; i++)
        row_dsc[i] = 16;
    row_dsc[sz] = LV_GRID_TEMPLATE_LAST;
    static lv_coord_t col_dsc[] = {lv_pct(30), lv_pct(30), lv_pct(40), LV_GRID_TEMPLATE_LAST};
    auto cont = lv_obj_create(cont_col);
    lv_obj_set_flex_grow(cont, 1);
    lv_obj_set_width(cont, lv_pct(100));
//	lv_obj_set_height(cont, lv_pct(100));
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);

    // Total memory
    auto total_memory_title = lv_label_create(cont);
    lv_obj_set_grid_cell(total_memory_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_label_set_text(total_memory_title, "Free memory");
    auto total_memory = lv_label_create(cont);
    lv_obj_set_grid_cell(total_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_label_set_text_fmt(total_memory, "%zu KB", m.free_memory / 1024);
    auto bar_free = lv_bar_create(cont);
    lv_obj_set_grid_cell(bar_free, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_center(bar_free);
    size_t pc = (m.free_memory * 100) / m.total_memory;
    lv_bar_set_value(bar_free, pc, LV_ANIM_OFF);
    lv_obj_add_style(bar_free, &style_bar, LV_STATE_DEFAULT);
    lv_obj_add_style(bar_free, &style_bar_indicator, LV_PART_INDICATOR);

    // Kernel? (Or a leak)
    auto kernel_used_title = lv_label_create(cont);
    lv_obj_set_grid_cell(kernel_used_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_label_set_text(kernel_used_title, "OS/D Kernel");
    auto kernel_used_memory = lv_label_create(cont);
    lv_obj_set_grid_cell(kernel_used_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_label_set_text_fmt(kernel_used_memory, "%zu KB", m.lost / 1024);
    auto bar_used = lv_bar_create(cont);
    lv_obj_set_grid_cell(bar_used, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_center(bar_used);
    pc = (m.lost * 100) / m.used;
    lv_bar_set_value(bar_used, pc, LV_ANIM_OFF);
    lv_obj_add_style(bar_used, &style_bar, LV_STATE_DEFAULT);
    lv_obj_add_style(bar_used, &style_bar_indicator, LV_PART_INDICATOR);

    size_t i = 2;

    // Show tasks
    for (auto &task: tasks_list) {

        // Title
        auto title = lv_label_create(cont);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
        lv_label_set_text_fmt(title, "%s", task->GetWindowName().c_str());

        // Memory
        auto memory = lv_label_create(cont);
        lv_obj_set_grid_cell(memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
        lv_label_set_text_fmt(memory, "%zu KB", task->CalculateMemoryUsed() / 1024);

        auto bar = lv_bar_create(cont);
        lv_obj_set_grid_cell(bar, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, i, 1);
        lv_obj_center(bar);
        size_t pc = (task->CalculateMemoryUsed() * 100) / m.used;
        lv_bar_set_value(bar, pc, LV_ANIM_OFF);
        lv_obj_add_style(bar, &style_bar, LV_STATE_DEFAULT);
        lv_obj_add_style(bar, &style_bar_indicator, LV_PART_INDICATOR);

        i++;
    }
    OSDTask::vlgl_mutex.unlock();
}