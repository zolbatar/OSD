#pragma once
#include "../../OS/OS.h"
#ifndef CLION
#include <circle/logger.h>
#endif

class FileManager : public OSDTask {
public:
	FileManager();
	void Run();
	static FSVolume* FindVolume(std::string volume);
private:
	FileSystemHandler fsFAT;
	FileSystemHandler* GetFSHandler(FileSystemType type);

	static std::map<std::string, FSVolume> volumes;
};