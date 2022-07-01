#include "FileManager.h"
#include "../Library/StringLib.h"
#include <circle/logger.h>

std::map<std::string, FSVolume> FileManager::volumes;
FileSystemHandler *FileManager::fsFAT;

FileManager::FileManager()
{
    this->id = "File Manager";
    this->name = "File Manager";
    this->priority = TaskPriority::Low;
};

void FileManager::Run()
{
    fsFAT = new FileSytemHandlerFAT();
    FSVolume boot;
    boot.prefix = "/osd/";
    volumes.insert(std::make_pair(":BOOT", std::move(boot)));
    CLogger::Get()->Write("File Manager", LogNotice, "Added volume '%s'", ":BOOT");

    FSVolume sd;
    sd.prefix = "/osd/home/";
    volumes.insert(std::make_pair(":HOME", std::move(sd)));
    CLogger::Get()->Write("File Manager", LogNotice, "Added volume '%s'", ":SD");

    fs.SetVolume(":BOOT");

    SetNameAndAddToList();
    while (1)
    {
        Yield();
    }
}

FileSystemHandler *FileManager::GetFSHandler(FileSystemType type)
{
    switch (type)
    {
    case FileSystemType::FAT:
        return fsFAT;
    }
    CLogger::Get()->Write("File Manager", LogPanic, "Unknown filesystem");
    return NULL;
}

FSVolume *FileManager::FindVolume(std::string volume)
{
    auto f = volumes.find(volume);
    if (f == volumes.end())
    {
        CLogger::Get()->Write("File Manager", LogPanic, "Volume '%s' not found", volume.c_str());
    }
    else
    {
        return &f->second;
    }
    CLogger::Get()->Write("File Manager", LogPanic, "Can't find volume");
    return NULL;
}

FileSystem::FileSystem()
{
}

void FileSystem::Init()
{
    this->handler = FileManager::GetFSHandler(FileSystemType::FAT);
}

FileSystem::FileSystem(FSVolume *volume)
{
    this->volume = volume;
}

void FileSystem::SetVolume(std::string volume)
{
    this->volume = FileManager::FindVolume(volume);
}

void FileSystem::SetCurrentDirectory(std::string directory)
{
    // Split everything by /
    auto split = splitString(directory, '/');

    // Do we have a drive specified?
    directory = "";
    if (split.front()[0] == ':')
    {
        SetVolume(split.front());
        split.pop_front();
    }
    directory += volume->prefix;

    // Now loop through all directories
    for (auto &dir : split)
    {
        directory += dir + "/";
    }

    // Check it's valid
    DIR dp;
    auto result = handler->OpenDirectory(directory, &dp);
    if (!result)
    {
        CLogger::Get()->Write("File Manager", LogPanic, "Directory '%s', not found, error code %d", directory.c_str(),
                              result);
    }
    f_closedir(&dp);
    current_directory = directory;
    // CLogger::Get()->Write("File Manager", LogNotice, "Directory '%s' set", directory.c_str());
}

std::vector<std::string> FileSystem::ListAllFilesInCurrentDirectory(bool subdirectories)
{
    std::vector<std::string> files;
    ListAllFilesInCurrentDirectoryWorker(subdirectories, current_directory, &files);
    return files;
}

void FileSystem::ListAllFilesInCurrentDirectoryWorker(bool subdirectories, std::string directory,
                                                      std::vector<std::string> *out)
{
    DIR dir;
    auto res = f_opendir(&dir, directory.c_str());
    if (res == FR_OK)
    {
        while (1)
        {
            FILINFO fno;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break;
            if (fno.fattrib & AM_DIR)
            {
                if (subdirectories)
                {
                    ListAllFilesInCurrentDirectoryWorker(subdirectories, directory + fno.fname + "/", out);
                }
            }
            else
            {
                std::string d;
                d += directory;
                d += fno.fname;
                out->push_back(d);
            }
        }
        f_closedir(&dir);
    }
}

std::vector<std::string> FileSystem::ListAllDirectoriesInCurrentDirectory(bool subdirectories, bool include_current)
{
    std::vector<std::string> files;
    ListAllDirectoriesInCurrentDirectoryWorker(subdirectories, current_directory, &files);
    if (include_current)
        files.push_back(current_directory);
    return files;
}

void FileSystem::ListAllDirectoriesInCurrentDirectoryWorker(bool subdirectories, std::string directory,
                                                            std::vector<std::string> *out)
{
    DIR dir;
    auto res = f_opendir(&dir, directory.c_str());
    if (res == FR_OK)
    {
        while (1)
        {
            static FILINFO fno;
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break;
            if (fno.fattrib & AM_DIR)
            {
                std::string d;
                d += directory;
                d += fno.fname;
                out->push_back(d);
                if (subdirectories)
                    ListAllDirectoriesInCurrentDirectoryWorker(subdirectories, directory + fno.fname + "/", out);
            }
        }
        f_closedir(&dir);
    }
}

bool FileSystemHandler::OpenDirectory(std::string directory, void *data)
{
    assert(0);
}

FileSytemHandlerFAT::FileSytemHandlerFAT()
{
    // Mount file system
    auto result = f_mount(&ff, "SD:", 1);
    if (result != FR_OK)
    {
        CLogger::Get()->Write("File Manager", LogPanic, "Can't mount SD card FAT filesystem, error code %d", result);
    }
    else
    {
        CLogger::Get()->Write("File Manager", LogNotice, "Initialised SD card FAT filesystem");
    }
}

bool FileSytemHandlerFAT::OpenDirectory(std::string directory, void *data)
{
    auto d = (DIR *)data;
    auto result = f_opendir(d, directory.c_str());
    return result == FR_OK;
}
