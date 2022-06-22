#pragma once
#ifndef CLION
#include <fatfs/ff.h>
#endif
#include <memory>

enum class FileSystemType {
	FAT
};

struct FSVolume {
	FileSystemType type;
	std::string prefix;
};

class FileSystemHandler {
public:
	virtual bool OpenDirectory(std::string directory, void* data);
};

class FileSytemHandlerFAT : public FileSystemHandler {
public:
	FileSytemHandlerFAT();
	bool OpenDirectory(std::string directory, void* data);
private:
	FATFS ff;
};

class FileSystem {
public:
	FileSystem();
	void Init();
	FileSystem(FSVolume* volume);
	void SetVolume(std::string volume);
	std::string GetCurrentDirectory() { return current_directory; }
	void SetCurrentDirectory(std::string directory);
	std::vector<std::string> ListAllFilesInCurrentDirectory(bool subdirectories);
	std::vector<std::string> ListAllDirectoriesInCurrentDirectory(bool subdirectories, bool include_current);
private:
	void ListAllFilesInCurrentDirectoryWorker(bool subdirectories, std::string directory, std::vector<std::string>* out);
	void ListAllDirectoriesInCurrentDirectoryWorker(bool subdirectories, std::string directory, std::vector<std::string>* out);

	std::unique_ptr<FileSystemHandler> handler;
	FSVolume* volume;
	std::string current_directory = "/";
};

