#include "TasksWindow.h"

extern size_t kernel_size;
extern size_t initial_mem_free;

TasksWindow::TasksWindow(int x, int y, int w, int h)
{
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->id = std::to_string(task_id++);
	this->name = "Task Manager";
	SetNameAndAddToList();
}

void TasksWindow::Run()
{
	// Create Window
	auto mess = SendGUIMessage();
	mess->type = Messages::WM_OpenWindow;
	mess->source = this;
	auto m = (WM_OpenWindow*)&mess->data;
	strcpy(m->id, "TASKS");
	strcpy(m->title, "Task Manager");
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
		Sleep(500);
	}

	TerminateTask();
}

void TasksWindow::UpdateTasks()
{
	if (cont!=NULL)
		lv_obj_del(cont);
	if (row_dsc!=NULL)
		free(row_dsc);

	auto w = ((Window*)this->GetWindow())->GetWindow();

	// Container
#ifndef CLION
	const int sz = tasks_list.size()+4;
#else
	const int sz = tasks_list.size();
#endif
	row_dsc = (lv_coord_t*)malloc(sizeof(lv_coord_t)*sz);
	for (int i = 0; i<sz; i++)
		row_dsc[i] = 16;
	row_dsc[sz] = LV_GRID_TEMPLATE_LAST;
	static lv_coord_t col_dsc[] = { lv_pct(30), lv_pct(30), lv_pct(40), LV_GRID_TEMPLATE_LAST };
	cont = lv_obj_create(lv_win_get_content(w));
	lv_obj_set_width(cont, lv_pct(100));
	lv_obj_set_height(cont, lv_pct(100));
	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
	lv_obj_center(cont);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);

	// Total memory
#ifndef CLION
	auto mem = CMemorySystem::Get();

	// Total memory
	auto total_memory_title = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(total_memory_title, "Total memory");
	auto total_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text_fmt(total_memory, "%zu bytes", mem->GetMemSize());

	// Available memory
	auto available_memory_title = lv_label_create(cont);
	lv_obj_set_grid_cell(available_memory_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text(available_memory_title, "Available memory");
	auto available_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(available_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text_fmt(available_memory, "%zu bytes", mem->GetHeapFreeSpace(HEAP_ANY));

	// Kernel? (Or a leak)
	auto kernel_used_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_label_set_text(kernel_used_title, "Total used");
	auto kernel_used_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	size_t used = initial_mem_free - mem->GetHeapFreeSpace(HEAP_ANY) + kernel_size;
	lv_label_set_text_fmt(kernel_used_memory, "%zu bytes", used);
	auto bar_used = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_used, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_obj_center(bar_used);
	size_t pc = (used * 100) / mem->GetMemSize();
	lv_bar_set_value(bar_used, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_used, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_used, &style_bar_indicator, LV_PART_INDICATOR);

	// Calc total task memory
	size_t total_task = kernel_size;
	for (auto& task: tasks_list) {
		total_task += task->CalculateMemoryUsed();
	}

	// Kernel? (Or a leak)
	auto kernel_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_label_set_text(kernel_title, "Kernel");
	auto kernel_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_label_set_text_fmt(kernel_memory, "%zu bytes", kernel_size);
	auto bar_kernel = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_kernel, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_obj_center(bar_kernel);
	pc = (kernel_size * 100) / total_task;
	lv_bar_set_value(bar_kernel, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_kernel, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_kernel, &style_bar_indicator, LV_PART_INDICATOR);

	size_t i = 4;
#else
	size_t i = 0;
	size_t total_task = 1;
#endif

	// Show tasks
	for (auto& task: tasks_list) {

		// Title
		auto title = lv_label_create(cont);
		lv_obj_set_grid_cell(title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_label_set_text_fmt(title, "%s", task->GetWindowName().c_str());

		// Memory
		auto memory = lv_label_create(cont);
		lv_obj_set_grid_cell(memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_label_set_text_fmt(memory, "%zu bytes", task->CalculateMemoryUsed());

		auto bar = lv_bar_create(cont);
		lv_obj_set_grid_cell(bar, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_obj_center(bar);
		size_t pc = (task->CalculateMemoryUsed()*100)/total_task;
		lv_bar_set_value(bar, pc, LV_ANIM_OFF);
		lv_obj_add_style(bar, &style_bar, LV_STATE_DEFAULT);
		lv_obj_add_style(bar, &style_bar_indicator, LV_PART_INDICATOR);

		i++;
	}
}