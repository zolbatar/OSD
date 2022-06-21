#pragma once
#include <list>
#include "../../OS/OS.h"
#include <circle/logger.h>
#include "../WindowManager/WindowManager.h"

struct FileIcon {
	std::string name;
	bool is_directory;
	std::string volume;
	std::string current_directory;
	FileType* type;
};

class Filer : public OSDTask {
public:
	Filer(std::string volume, std::string directory);
	void Run();

private:
	const int cell_size = 96;
	std::string volume;
	std::string directory;
	static int cx;
	static int cy;
	lv_obj_t* filer_cont;
	std::list<FileIcon> icons;
	static FileIcon* icon_clicked;
	static unsigned last_click;

	void AddIcon(std::string name, bool is_directory);
	static void IconClickEventHandler(lv_event_t* e);
};
