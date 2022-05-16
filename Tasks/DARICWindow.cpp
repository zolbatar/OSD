#include "DARICWindow.h"

DARICWindow::DARICWindow(std::string id, std::string name, bool exclusive, int x, int y, int w, int h)
		:id(id), name(name), exclusive(exclusive), x(x), y(y), w(w), h(h)
{
}

void DARICWindow::SetSourceCode(std::string code)
{
	this->code = code;
}

void DARICWindow::Run()
{
#ifndef CLION
	SetName(id.c_str());
#else
	SetName(id);
#endif

	// Create Window
	WM_OpenWindow* m = new WM_OpenWindow();
	m->id = id;
	m->title = name;
	m->x = x;
	m->y = y;
	m->width = w;
	m->height = h;
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