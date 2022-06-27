#include <circle/logger.h>
#include "Editor.h"
#include "../../Library/StringLib.h"
#include "../../GUI/Window/LVGLWindow.h"

Editor::Editor(int x, int y, int w, int h)
{
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	auto window_border_width = ThemeManager::GetConst(ConstAttribute::WindowBorderWidth);
	auto window_content_padding = ThemeManager::GetConst(ConstAttribute::WindowContentPadding);
	auto window_header_height = ThemeManager::GetConst(ConstAttribute::WindowHeaderHeight);
	this->canvas_w = w-window_border_width*2-window_content_padding*2;
	this->canvas_h = h-window_border_width*2-window_header_height-window_content_padding*2;
	this->id = "@"+std::to_string(task_id++);
	this->name = "Editor";
}

void Editor::Run()
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
	m.canvas = true;
	m.canvas_w = canvas_w;
	m.canvas_h = canvas_h;
	m.fixed = true;
	CallGUIDirectEx(&mess);

	// Set up canvas
	auto window = (Window*)this->GetWindow();
	auto ww = window->GetLVGLWindow();
	auto content = lv_mywin_get_content(ww);
	obj = lv_obj_create(content);
	lv_obj_add_style(obj, ThemeManager::GetStyle(StyleAttribute::TransparentWindow), LV_STATE_DEFAULT);

	// Set canvas to float
	auto canvas = window->GetCanvas();
	lv_obj_add_flag(canvas->GetFirstBuffer(), LV_OBJ_FLAG_FLOATING);
	lv_obj_align(canvas->GetFirstBuffer(), LV_ALIGN_TOP_LEFT, 0, 0);

	// Scroll event
	lv_obj_add_event_cb(content, ScrollEventHandler, LV_EVENT_SCROLL, this);

	// Build
	Render();

	// Do stuff
	while (1) {
		Yield();
	}
}

/*void Editor::TextareaEventHandler(lv_event_t* e)
{
	lv_obj_t* ta = lv_event_get_target(e);
	LV_LOG_USER("Enter was pressed. The current text is: %s", lv_textarea_get_text(ta));
}*/

void Editor::LoadSourceCode(std::string volume, std::string directory, std::string filename)
{
	SetOverride(this);
	fs.SetVolume(volume);
	fs.SetCurrentDirectory(directory);
	filename = fs.GetCurrentDirectory()+filename;

	FIL fil;
	if (f_open(&fil, (filename).c_str(), FA_READ | FA_OPEN_EXISTING)!=FR_OK) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error opening source file '%s'", filename.c_str());
	}
	size_t sz = f_size(&fil);
	char* buffer = (char*)malloc(sz+1);
	if (!buffer) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error allocating memory for source file '%s'", filename.c_str());
	}
	uint32_t l;
	if (f_read(&fil, buffer, sz, &l)!=FR_OK) {
		CLogger::Get()->Write("OSDTask", LogPanic, "Error loading source file '%s'", filename.c_str());
	}
	f_close(&fil);
	buffer[sz] = 0;
	std::string s(buffer);
	free(buffer);

	// Split into lines
	auto ll = splitString(s, '\n');
	code.reserve(ll.size());
	std::copy(std::begin(ll), std::end(ll), std::back_inserter(code));
	CalculateLongestLine();

	ClearOverride();
}

void Editor::CalculateLongestLine()
{
	longest_line = 0;
	for (auto& c : code) {
		longest_line = std::max(longest_line, c.length());
	}
}

void Editor::Render()
{
	auto window = (Window*)this->GetWindow();
	auto canvas = window->GetCanvas();
	auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
	lv_obj_set_size(obj, longest_line*size, code.size()*size);

	// Clear
	canvas->Clear();

	int actual_y = 0;
	for (size_t i = y; i<code.size(); i++) {

		// Get line
		auto line = code[i];

		int actual_x = 0;
		for (size_t j = x; j<line.length(); j++) {
			char c = line[j];

			// Draw
			canvas->DrawText(actual_x, actual_y, std::string(1, c));

			// Next character
			actual_x += size/2;
			if (actual_x>canvas_w)
				break;
		}

		// Next line
		actual_y += size;
		if (actual_y>canvas_h)
			break;
	}
}

void Editor::ScrollEventHandler(lv_event_t* e)
{
	lv_obj_t* scroll = lv_event_get_target(e);
	auto scroll_y = lv_obj_get_scroll_top(scroll);
	auto scroll_x = lv_obj_get_scroll_left(scroll);

	// Work out X and Y
	auto editor = (Editor*)e->user_data;
	auto size = ThemeManager::GetConst(ConstAttribute::MonoFontSize);
	editor->x = scroll_x/size/2;
	editor->y = scroll_y/size;
	if (editor->x<0) editor->x = 0;
	if (editor->y<0) editor->y = 0;

	// Render
	editor->Render();
	CLogger::Get()->Write("File Manager", LogNotice, "Scroll %d %d: %d %d", scroll_x, scroll_y, editor->x, editor->y);
}
