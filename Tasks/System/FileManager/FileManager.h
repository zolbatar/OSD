#pragma once
#include "../../../OS/OS.h"
#include <circle/logger.h>

class FileManager : public OSDTask
{
  public:
    FileManager();
    void Run();
    static FSVolume *FindVolume(std::string volume);
    static FileSystemHandler *GetFSHandler(FileSystemType type);
    static std::string BootDrivePrefix;

  private:
    static FileSystemHandler *fsFAT;

    static std::map<std::string, FSVolume> volumes;
};