#include "Editor.h"

Editor::Editor(int x, int y, int w, int h)
{
	this->d_x = x;
	this->d_y = y;
	this->d_w = w;
	this->d_h = h;
	this->id = "@"+std::to_string(task_id++);
	this->name = "Editor";
	type = TaskType::Editor;
#ifndef CLION
	this->SetUserData(&type, TASK_USER_DATA_USER);
#endif
}

void Editor::Run()
{
	SetNameAndAddToList();

	// Create Window
	auto mess = SendGUIMessage();
	mess->type = Messages::WM_OpenWindow;
	mess->source = this;
	auto m = (WM_OpenWindow*) & mess->data;
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
		Sleep(1000);
	}

	TerminateTask();
}