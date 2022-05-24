#include "TasksWindow.h"

extern size_t pre_boot_memory;
extern size_t kernel_size;
extern size_t body_font_height;

TasksWindow::TasksWindow(int x, int y, int w, int h)
{
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->id = "@"+std::to_string(task_id++);
	this->name = "Task Manager";
	type = TaskType::TaskManager;
#ifndef CLION
	this->SetUserData(&type, TASK_USER_DATA_USER);
#endif
}

void TasksWindow::Run()
{
	SetNameAndAddToList();

	// Create Window
	auto mess = SendGUIMessage();
	mess->type = Messages::WM_OpenWindow;
	mess->source = this;
	auto m = (WM_OpenWindow*)&mess->data;
	strcpy(m->id, this->id.c_str());
	strcpy(m->title, this->name.c_str());
	m->x = d_x;
	m->y = d_y;
	m->width = d_w;
	m->height = d_h;
	m->canvas = false;
	m->fixed = false;

	// Wait for window to be created
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w==NULL);

	// Do stuff
	while (1) {
		UpdateTasks();
		Sleep(1000);
	}

	TerminateTask();
}

void TasksWindow::UpdateTasks()
{
	MemorySummary m;
	CalculateMem(&m);

	LockVLGL("TasksWindow::UpdateTasks");
	auto w = ((Window*)this->GetWindow())->GetLVGLWindow();
	lv_obj_clean(lv_win_get_content(w));

	// Vertical container
	auto cont_col = lv_obj_create(lv_win_get_content(w));
	lv_obj_set_width(cont_col, lv_pct(100));
	lv_obj_set_height(cont_col, lv_pct(100));
	lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(cont_col, &style_grid, LV_STATE_DEFAULT);

	// Container
	const int sz = tasks_list.size()+3;
	static lv_coord_t row_dsc[256];
	for (int i = 0; i<sz; i++)
		row_dsc[i] = body_font_height;
	row_dsc[sz] = LV_GRID_TEMPLATE_LAST;
	static lv_coord_t col_dsc[] = { lv_pct(30), lv_pct(30), lv_pct(40), LV_GRID_TEMPLATE_LAST };
	auto cont = lv_obj_create(cont_col);
	lv_obj_set_flex_grow(cont, 2);
	lv_obj_set_width(cont, lv_pct(100));
	lv_obj_set_height(cont, lv_pct(100));
	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
	lv_obj_center(cont);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);

	// Total memory
	auto total_memory_title = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(total_memory_title, "Free Memory");
	auto total_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text_fmt(total_memory, "%zu KB", m.free_memory/1024);
	auto bar_free = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_free, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_center(bar_free);
	size_t pc = (m.free_memory*100)/m.total_memory;
	lv_bar_set_value(bar_free, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_free, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_free, &style_bar_indicator_green, LV_PART_INDICATOR);

	// Kernel size
	auto kernel_size_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_size_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text(kernel_size_title, "Boot Size");
	auto kernel = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text_fmt(kernel, "%zu KB", (kernel_size+pre_boot_memory)/1024);

	// Kernel? (Or a leak)
	auto kernel_used_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_label_set_text(kernel_used_title, "OS/D Kernel & Pool");
	auto kernel_used_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_label_set_text_fmt(kernel_used_memory, "%zu KB", m.lost/1024);
	auto bar_used = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_used, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_obj_center(bar_used);
	pc = (m.lost*100)/m.used;
	lv_bar_set_value(bar_used, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_used, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_used, &style_bar_indicator_red, LV_PART_INDICATOR);

	size_t i = 3;

	// Show tasks
	for (auto& task: tasks_list) {

		// Title
		auto title = lv_label_create(cont);
		lv_obj_set_grid_cell(title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		if (task->GetWindowName()=="@") {
			lv_label_set_text_fmt(title, "Window Manager");
		}
		else {
			if (task->GetWindowID()[0]!='@') {
				// Module
				lv_label_set_text_fmt(title, "%s", task->GetWindowName().c_str());
			}
			else {
				// Task
				lv_label_set_text_fmt(title, "%s [%s]", task->GetWindowName().c_str(), task->GetWindowID().c_str());
			}
		}

		// Memory
		auto memory = lv_label_create(cont);
		lv_obj_set_grid_cell(memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_label_set_text_fmt(memory, "%zu KB [%zu %zu %zu]", task->CalculateMemoryUsed()/1024, task->GetStringCount(), task->GetAllocCount(),
				task->GetMessageQueueCount());

		auto bar = lv_bar_create(cont);
		lv_obj_set_grid_cell(bar, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_obj_center(bar);
		size_t pc = (task->CalculateMemoryUsed()*100)/m.used;
		lv_bar_set_value(bar, pc, LV_ANIM_OFF);
		lv_obj_add_style(bar, &style_bar, LV_STATE_DEFAULT);
		lv_obj_add_style(bar, &style_bar_indicator_red, LV_PART_INDICATOR);

		i++;
	}
	UnlockVLGL();
}