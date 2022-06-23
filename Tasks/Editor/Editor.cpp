#include "Editor.h"
#include "../../Library/StringLib.h"
#include <fstream>
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
	m.canvas = true;
	m.fixed = true;
	CallGUIDirectEx(&mess);

	// Build
	auto ww = ((Window*)this->GetWindow())->GetLVGLWindow();
	lv_obj_t* ta = lv_textarea_create(lv_mywin_get_content(ww));
	lv_textarea_set_one_line(ta, true);
	lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_add_event_cb(ta, TextareaEventHandler, LV_EVENT_READY, ta);
	lv_obj_set_width(ta, lv_pct(100));
	lv_obj_set_height(ta, lv_pct(100));
	lv_textarea_add_text(ta, code.c_str());
	lv_obj_add_state(ta, LV_STATE_FOCUSED);
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

void Editor::LoadSourceCode(std::string directory, std::string filename)
{
	std::vector<std::string> lines;

	// Quick and dirty file stuff until we have a proper file manager
#ifndef CLION
	replace(filename, ":SD.$.Welcome.", "/osd/Welcome/");
#else
	replace(filename, ":SD.$.Welcome.", "/Users/daryl/GitHub/osd/Applications/");
#endif

// Open and check exists
	std::ifstream in(filename);
	if (!in.is_open()) {
#ifndef CLION
		CLogger::Get()->Write("DARICWindow", LogDebug, "Error opening source file: %s", filename.c_str());
#else
		printf("Error opening source file\n");
#endif
		assert(0);
	}

// Read all lines
	std::string line;
	while (std::getline(in, line)) {
		lines.push_back(line);
	}

//  Concatenate
	std::string s;
	for (const auto& line : lines) s += line+'\n';
	this->code = s;
}

void Editor::UpdateGUI()
{
}


