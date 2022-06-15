#include "DARICWindow.h"
#include "../Exception/DARICException.h"
#include <memory.h>
#include <string.h>
#include <fstream>

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

void DARICWindow::LoadSourceCode(std::string filename)
{
	this->filename = filename;
	this->code = this->LoadSource(filename);
}

void DARICWindow::Run()
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
	Yield();

	// Compile (and run)
	try {
		if (CompileSource(filename, code)) {
			//CLogger::Get()->Write("DARICWindow", LogDebug, "Run: %s", GetWindowName().c_str());
			RunCode();
		}
	}
	catch (DARICException& ex) {
		std::string cat;
		switch (ex.type) {
			case ExceptionType::COMPILER:
				cat = "[Compiler] ";
				break;
			case ExceptionType::TOKENISER:
				cat = "[Tokeniser] ";
				break;
			case ExceptionType::PARSER:
				cat = "[Parser] ";
				break;
			case ExceptionType::RUNTIME:
				cat = "[Runtime] ";
				break;
		}
#ifdef CLION
		printf("%s%s in file '%s' at line %d, column %d\n", cat.c_str(), ex.error.c_str(), ex.filename.c_str(), ex.line_number, ex.char_position);
#else
		CLogger::Get()->Write("CompileSource", LogPanic, "%s%s in file '%s' at line %d, column %d", cat.c_str() ,ex.error.c_str(), ex.filename.c_str(), ex.line_number, ex.char_position);
#endif
	}

	TerminateTask();
}

void DARICWindow::UpdateGUI()
{
//	CLogger::Get()->Write("DARICWindow", LogDebug, "Render");
//	auto w = (Window*)GetWindow();
//	auto c = w->GetCanvas();
//	c->Render();
//	lv_obj_invalidate(lv_scr_act());
}
