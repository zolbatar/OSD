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
	m.canvas = false;
	m.fixed = true;
	CallGUIDirectEx(&mess);

	// Build
	auto ww = ((Window*)this->GetWindow())->GetLVGLWindow();
	lv_obj_t* ta = lv_textarea_create(lv_mywin_get_content(ww));
//	lv_textarea_set_one_line(ta, true);
	lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_event_cb(ta, TextareaEventHandler, LV_EVENT_READY, ta);
	lv_obj_set_width(ta, lv_pct(100));
	lv_obj_set_height(ta, lv_pct(100));
	lv_textarea_add_text(ta, code.c_str());
//	lv_obj_add_state(ta, LV_STATE_FOCUSED);
//	lv_obj_add_style(ta, &style_textarea, LV_STATE_DEFAULT);

	// Do stuff
	while (1) {
		Yield();
	}
}

void Editor::TextareaEventHandler(lv_event_t* e)
{
	lv_obj_t* ta = lv_event_get_target(e);
	LV_LOG_USER("Enter was pressed. The current text is: %s", lv_textarea_get_text(ta));
}

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
	this->code = s;
	ClearOverride();
}


