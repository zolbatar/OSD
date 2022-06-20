#pragma once
#include "../../OS/OS.h"
#include <circle/logger.h>

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