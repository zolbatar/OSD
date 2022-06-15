#pragma once
#include "../../OS/OS.h"
#include "FileSystemObject.h"

#ifndef CLION
#include <circle/logger.h>
#endif

class FileManager : public OSDTask {
public:
	FileManager();
	void Run();
private:
};