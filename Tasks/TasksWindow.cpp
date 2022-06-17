#include "TasksWindow.h"
#include "../GUI/Window/LVGLWindow.h"

extern size_t pre_boot_memory;
extern size_t kernel_size;
extern size_t body_font_height;
extern int dm;

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
	DirectMessage mess;
	mess.type = Messages::WM_OpenWindow;
	mess.source = this;
	WM_OpenWindow m;
	mess.data = &m;
	strcpy(m.id, id.c_str());
	strcpy(m.title, name.c_str());
	m.x = d_x;
	m.y = d_y;
	m.width = d_w;
	m.height = d_h;
	m.canvas = false;
	m.fixed = true;
	CallGUIDirectEx(&mess);

	// Do stuff
	while (1) {
		UpdateGUI();
		Sleep(1000);
	}

	TerminateTask();
}

void TasksWindow::UpdateGUI()
{
	MemorySummary m;
	CalculateMem(&m);

	auto w = ((Window*)this->GetWindow())->GetLVGLWindow();
	lv_obj_clean(lv_mywin_get_content(w));

	// Container
	const int sz = tasks_list.size()+4;
	static lv_coord_t row_dsc[256];
	row_dsc[0] = (body_font_height+4)*dm;
	for (int i = 1; i<sz; i++)
		row_dsc[i] = body_font_height*dm;
	row_dsc[sz] = LV_GRID_TEMPLATE_LAST;
	static lv_coord_t col_dsc[] = { lv_pct(41), lv_pct(15), lv_pct(7), lv_pct(7), lv_pct(30), LV_GRID_TEMPLATE_LAST };

	// Vertical container
	auto cont = lv_obj_create(lv_mywin_get_content(w));
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
//	lv_obj_set_width(cont, LV_SIZE_CONTENT);
	lv_obj_set_height(cont, LV_SIZE_CONTENT);
	lv_obj_set_width(cont, lv_pct(100));
//	lv_obj_set_height(cont, lv_pct(100));
	//	lv_obj_align(cont_col, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);
	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_add_style(cont, &style_scrollbar, LV_PART_SCROLLBAR);
	lv_obj_add_style(cont, &style_grid, LV_STATE_DEFAULT);

	// Title
	auto title1 = lv_label_create(cont);
	lv_obj_set_grid_cell(title1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(title1, "Task");
	lv_obj_add_style(title1, &style_grid_title, LV_STATE_DEFAULT);
	auto title2 = lv_label_create(cont);
	lv_obj_set_grid_cell(title2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(title2, "Memory");
	lv_obj_add_style(title2, &style_grid_title, LV_STATE_DEFAULT);
	auto title3 = lv_label_create(cont);
	lv_obj_set_grid_cell(title3, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(title3, "Framebuffer");
	lv_obj_add_style(title3, &style_grid_title, LV_STATE_DEFAULT);
	auto title4 = lv_label_create(cont);
	lv_obj_set_grid_cell(title4, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_label_set_text(title4, "Allocations");
	lv_obj_add_style(title4, &style_grid_title, LV_STATE_DEFAULT);

	// Total memory
	auto total_memory_title = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text(total_memory_title, "Free Memory");
	auto total_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(total_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_label_set_text_fmt(total_memory, "%zu KB", m.free_memory/1024);
	auto bar_free = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_free, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_center(bar_free);
	size_t pc = (m.free_memory*100)/m.total_memory;
	lv_bar_set_value(bar_free, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_free, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_free, &style_bar_indicator_green, LV_PART_INDICATOR);

	// Kernel size
	auto kernel_size_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_size_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_label_set_text(kernel_size_title, "Boot Size");
	auto kernel = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
	lv_label_set_text_fmt(kernel, "%zu KB", (kernel_size+pre_boot_memory)/1024);

	// Kernel? (Or a leak)
	auto kernel_used_title = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_title, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_label_set_text(kernel_used_title, "OS/D Kernel & Pool");
	auto kernel_used_memory = lv_label_create(cont);
	lv_obj_set_grid_cell(kernel_used_memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_label_set_text_fmt(kernel_used_memory, "%zu KB", m.lost/1024);
	auto bar_used = lv_bar_create(cont);
	lv_obj_set_grid_cell(bar_used, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_STRETCH, 3, 1);
	lv_obj_center(bar_used);
	pc = (m.lost*100)/m.used;
	lv_bar_set_value(bar_used, pc, LV_ANIM_OFF);
	lv_obj_add_style(bar_used, &style_bar, LV_STATE_DEFAULT);
	lv_obj_add_style(bar_used, &style_bar_indicator_red, LV_PART_INDICATOR);

	size_t i = 4;

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
		auto w = (Window*)task->GetWindow();
		if (w!=NULL) {
			if (w->GetActive()) {
				lv_obj_add_style(title, &style_boldbodyfont, LV_STATE_DEFAULT);
			}
		}

		// Memory
		auto memory = lv_label_create(cont);
		lv_obj_set_grid_cell(memory, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		auto total = task->CalculateMemoryUsed();
		auto fb = task->GetFrameBufferMemory();
		if (total<fb) total = fb;
		lv_label_set_text_fmt(memory, "%zu KB", (total-fb)/1024);

		// Framebuffer
		auto memory2 = lv_label_create(cont);
		lv_obj_set_grid_cell(memory2, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_label_set_text_fmt(memory2, "%zu KB", fb/1024);

		// Allocations
		auto memory3 = lv_label_create(cont);
		lv_obj_set_grid_cell(memory3, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_label_set_text_fmt(memory3, "%zu [%zu+%zu]", task->GetAllocCount(), task->GetStringCount(), task->GetStringCountTemporary());

		auto bar = lv_bar_create(cont);
		lv_obj_set_grid_cell(bar, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_STRETCH, i, 1);
		lv_obj_center(bar);
		size_t pc = (task->CalculateMemoryUsed()*100)/m.used;
		lv_bar_set_value(bar, pc, LV_ANIM_OFF);
		lv_obj_add_style(bar, &style_bar, LV_STATE_DEFAULT);
		lv_obj_add_style(bar, &style_bar_indicator_red, LV_PART_INDICATOR);

		i++;
	}
}