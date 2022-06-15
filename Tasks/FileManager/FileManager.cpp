#include "FileManager.h"
#include "../Library/StringLib.h"
#include <fatfs/ff.h>
#ifndef CLION
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

void FileSystem::SetCurrentDirectory(std::string directory)
{
	switch (format) {
		case FileNamingFormat::Acorn: {

			// Split everything by .
			auto split = splitString(directory, '.');
			for (auto& a : split) {
				CLogger::Get()->Write("FileSystem", LogDebug, "%s", a.c_str());
			}
			while (1);

			// Do we have a drive specified?
			if (directory[0]==':') {
			}

		}
		default:
			assert(0);
	}
}
