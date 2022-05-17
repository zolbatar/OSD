#include "DARICWindow.h"

DARICWindow::DARICWindow(std::string name, bool exclusive, int x, int y, int w, int h)
{
	this->exclusive = exclusive;
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->name = name;
	this->id = std::to_string(task_id++);
	SetNameAndAddToList();
}

void DARICWindow::SetSourceCode(std::string code)
{
	this->code = code;
}

void DARICWindow::Run()
{
	// Create Window
	WM_OpenWindow* m = NEW WM_OpenWindow();
	m->id = id;
	m->title = name;
	m->x = d_x;
	m->y = d_y;
	m->width = d_w;
	m->height = d_h;
	m->canvas = true;
	m->fixed = true;
	SendGUIMessage(Messages::WM_OpenWindow, (void*)m);

	// Wait for window to be created
	Window* w;
	do {
		Yield();
		w = (Window*)GetWindow();
	}
	while (w==NULL);

	// Compile (and run)
	CompileSource(code);

	TerminateTask();
}