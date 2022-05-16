#include "TasksWindow.h"

TasksWindow::TasksWindow(int x, int y, int w, int h) {
    this->d_x = x;
    this->d_y = y;
    this->d_w = w;
    this->d_h = h;
    this->id = std::to_string(task_id++);
    SetName("Task Manager");
}

void TasksWindow::Run() {

    // Create Window
    WM_OpenWindow *m = new WM_OpenWindow();
    m->id = "TASKS";
    m->title = "Task Manager";
    m->x = d_x;
    m->y = d_y;
    m->width = d_w;
    m->height = d_h;
    m->canvas = false;
    m->fixed = false;
    SendGUIMessage(Messages::WM_OpenWindow, (void *) m);

    // Wait for window to be created
    Window *w;
    do {
        Yield();
        w = (Window *) GetWindow();
    } while (w == NULL);

    // Do stuff
    while (1) {
        UpdateTasks();
        Yield();
    }

    TerminateTask();
}

void TasksWindow::UpdateTasks() {
    if (cont != NULL)
        lv_obj_del(cont);
    if (row_dsc != NULL)
        free(row_dsc);

    auto w = ((Window *) this->GetWindow())->GetWindow();

    // Container
    row_dsc = (lv_coord_t *) malloc(sizeof(lv_coord_t) * (tasks_list.size() + 1));
    for (auto i = 0; i < tasks_list.size(); i++)
        row_dsc[i] = 20;
    row_dsc[tasks_list.size()] = LV_GRID_TEMPLATE_LAST;
    static lv_coord_t col_dsc[] = {100, 50, LV_GRID_TEMPLATE_LAST};
    cont = lv_obj_create(lv_win_get_content(w));
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 200, 200);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);

    // Show tasks
    auto i = 0;
    for (auto &task: tasks_list) {

        // Title
        auto title = lv_label_create(cont);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
        lv_label_set_text_fmt(title, "%s", task->GetWindowName().c_str());

        // Memory
        auto memory = lv_label_create(cont);
        lv_obj_set_grid_cell(memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
        lv_label_set_text(memory, "1");

        i++;
    }
}