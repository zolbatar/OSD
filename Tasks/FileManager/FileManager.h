#pragma once
#include "../../OS/OS.h"
#include "FileSystemObject.h"

#ifndef CLION
#include <circle/logger.h>
#endif

class FileSystemHandler {
public:
};

class FileManager : public OSDTask {
public:
	FileManager();
	void Run();
	FileSystemHandler* GetFSHandler(std::string fs);
private:
	FileSystemHandler sd_fs;
};