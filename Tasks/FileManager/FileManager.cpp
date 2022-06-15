#include "FileManager.h"
#include "../Library/StringLib.h"
#ifndef CLION
#include <fatfs/ff.h>
#include <circle/logger.h>
#endif

FileManager::FileManager()
{
	this->id = "File Manager";
	this->name = "File Manager";
	this->priority = TaskPriority::Low;
};

void FileManager::Run()
{
	SetNameAndAddToList();
	while (1) {
		Yield();
	}
	TerminateTask();
}

FileSystemHandler* FileManager::GetFSHandler(std::string fs)
{
	if (fs==":sd") {
		return &sd_fs;
	}
	else {
#ifdef CLION
		printf("File system '%s' not found", fs.c_str());
		exit(1);
#else
		CLogger::Get()->Write("File Manager", LogPanic, "File system '%s' not found", fs.c_str());
#endif
	}
}

void FileSystem::SetCurrentDirectory(std::string directory)
{
	switch (format) {
		case FileNamingFormat::Acorn: {

			// Split everything by .
			auto split = splitString(directory, '.');
			for (auto& a : split) {
//				CLogger::Get()->Write("File Manager", LogDebug, "%s", a.c_str());
			}

			// Do we have a drive specified?
			if (split[0][0]==':') {
//				CLogger::Get()->Write("File Manager", LogDebug, "Filesystem: %s", split[0].c_str());
			}

			while (1);

		}
		default:
			assert(0);
	}
}
