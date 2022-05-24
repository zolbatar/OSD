#include "DARICWindow.h"
#include <memory.h>
#include <string.h>
#include <fstream>
#include "../Library/StringLib.h"

DARICWindow::DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h)
{
	this->exclusive = exclusive;
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->name = name;
	this->id = "@"+std::to_string(task_id++);
	type = TaskType::DARIC;
#ifndef CLION
	SetUserData(&type, TASK_USER_DATA_USER);
#endif

}

void DARICWindow::SetSourceCode(std::string code)
{
	task_override = this;
	this->code = code;
	task_override = NULL;
}

void DARICWindow::LoadSourceCode(std::string filename)
{
	task_override = this;
	std::vector<std::string> lines;

	// Quick and dirty file stuff until we have a proper file manager
	replace(filename, ":SD.$.Welcome.", "/osd/Welcome/");

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
	task_override = NULL;
}

void DARICWindow::Run()
{
	SetNameAndAddToList();

	// Create Window
	auto mess = SendGUIMessage();
	mess->type = Messages::WM_OpenWindow;
	mess->source = this;
	auto m = (WM_OpenWindow*)&mess->data;
	strcpy(m->id, id.c_str());
	strcpy(m->title, name.c_str());
	m->x = d_x;
	m->y = d_y;
	m->width = d_w;
	m->height = d_h;
	m->canvas = true;
	m->fixed = true;

	// Wait for window to be created
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w==NULL);

	// Compile (and run)
	CompileSource(code);
	code = "";
	RunCode();

	TerminateTask();
}
