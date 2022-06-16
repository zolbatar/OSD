#pragma once
#ifndef CLION
#include <fatfs/ff.h>
#endif
#include <memory>

enum class FileNamingFormat {
	Acorn
};

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
	void SetCurrentDirectory(std::string directory);
	std::vector<std::string> ListAllFilesInCurrentDirectory(bool subdirectories);
	std::vector<std::string> ListAllDirectoriesInCurrentDirectory();
private:
	void ListAllFilesInCurrentDirectoryWorker(bool subdirectories, std::string directory, std::vector<std::string>* out);

	FileNamingFormat format = FileNamingFormat::Acorn;
	std::unique_ptr<FileSystemHandler> handler;
	FSVolume* volume;
	std::string current_directory = "/";
};

