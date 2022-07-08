#pragma once
#include <OS_Includes.h>

extern "C"
{
#include "../Compiler/Lightning/lightning.h"
#include "../Compiler/Lightning/jit_private.h"
}

typedef void (*start)(void);

class OSDTaskCode
{
  public:
    OSDTaskCode();
    ~OSDTaskCode();
    std::string LoadSource(FileSystem *fs, std::string volume, std::string directory, std::string filename);
    std::string CompileSource(FileSystem *fs, std::string volume, std::string directory, std::string filename,
                              std::string code, bool debug);
    void RunCode(bool wait);
    void SetStart(start s);
    void CreateCode(size_t code_size);
    uint8_t *GetCode();
    size_t GetCodeSize()
    {
        return code_size;
    }
    start GetExec();
    bool WaitAtEnd()
    {
        return wait;
    }
    int64_t GetT1()
    {
        return t1;
    }

  private:
    int64_t t1;
    bool wait;
    start exec = NULL;
    size_t code_size;
    size_t data_size;
    uint8_t *code = NULL;
    jit_state_t *_jit = NULL;
};