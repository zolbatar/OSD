#include "DARICWindow.h"
#include <memory.h>
#include <string.h>
#ifndef CLION
#include <circle/logger.h>
#endif

DARICWindow::DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h)
{
	this->exclusive = exclusive;
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->name = name;
	this->id = "@"+std::to_string(task_id++);
}

void DARICWindow::SetSourceCode(std::string code)
{
	this->code = code;
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
	RunCode();

	TerminateTask();
}