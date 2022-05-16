#include "TasksWindow.h"

TasksWindow::TasksWindow(int x, int y, int w, int h)
{
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
}

void TasksWindow::Run()
{
	SetName("Tasks");

	// Create Window
	WM_OpenWindow* m = new WM_OpenWindow();
	m->id = "TASKS";
	m->title = "Tasks";
	m->x = d_x;
	m->y = d_y;
	m->width = d_w;
	m->height = d_h;
	m->canvas = false;
	m->fixed = false;
	SendGUIMessage(Messages::WM_OpenWindow, (void*)m);

	// Wait for window to be created
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w==NULL);

	// Container
	static lv_coord_t col_dsc[] = { 70, 70, 70, LV_GRID_TEMPLATE_LAST };
	static lv_coord_t row_dsc[] = { 50, 50, 50, LV_GRID_TEMPLATE_LAST };
	cont = lv_obj_create(lv_win_get_content(w->GetWindow()));
	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
//	lv_obj_set_size(cont, 500, 220);
	lv_obj_center(cont);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	// Do stuff
	while (1) {
		UpdateTasks();
		Yield();
	}

	TerminateTask();
}

void TasksWindow::UpdateTasks()
{
	lv_obj_clean(cont);

	uint32_t i;
	for (auto& task: tasks_list) {

		// Title
		auto btn_title = lv_btn_create(cont);
		lv_obj_set_grid_cell(btn_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		auto label_title = lv_label_create(btn_title);
		lv_label_set_text_fmt(label_title, "%s", task->GetWindowName().c_str());

		// Memory
		auto btn_memory = lv_btn_create(cont);
		lv_obj_set_grid_cell(btn_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		auto memory_title = lv_label_create(btn_memory);
		lv_label_set_text_fmt(memory_title, "100K");
		lv_obj_center(label_title);

		i++;
	}
}