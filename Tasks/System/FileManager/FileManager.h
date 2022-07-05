#pragma once
#include "../../OS/OS.h"
#include <circle/logger.h>

class FileManager : public OSDTask
{
  public:
    FileManager();
    void Run();
    static FSVolume *FindVolume(std::string volume);
    static FileSystemHandler *GetFSHandler(FileSystemType type);
    static std::string GetDrivePrefix()
    {
        return PHY_DRV_PREFIX;
    }

  private:
    static FileSystemHandler *fsFAT;

    static std::map<std::string, FSVolume> volumes;
    static std::string PHY_DRV_PREFIX;
};