#include "NativeCompiler.h"
#include <capstone/capstone.h>
#include <capstone/platform.h>

void NativeCompiler::Disassemble(std::map<size_t, std::string> *disass)
{
    CLogger::Get()->Write("NativeCompiler", LogNotice, "Disassembly");

    // Disassemble
    csh handle;
    cs_insn *insn;
    size_t count;
    cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
    if (err != CS_ERR_OK)
    {
        CLogger::Get()->Write("NativeCompiler", LogPanic, "Disassemble error: ", err);
    }
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    auto exec = (uint8_t *)task->GetExec();
    count = cs_disasm(handle, exec, task->GetCodeSize(), (uint64_t)_jit->code.ptr, 0, &insn);
    char f[256];
    if (count > 0)
    {
        if (OSDTask::yield_due)
            GetCurrentTask()->Yield();
        size_t j;
        for (j = 0; j < count; j++)
        {
            sprintf(f, "0x%" PRIx64 ":\t%s\t%s", insn[j].address, insn[j].mnemonic, insn[j].op_str);
            auto ss = std::string(f);
            replaceAll(ss, "\t", " ");
            disass->insert(std::make_pair(insn[j].address, ss));
            // CLogger::Get()->Write("NativeCompiler", LogNotice, "%p: %s", insn[j].address, f);
        }

        cs_free(insn, count);
    }
    else
    {
        CLogger::Get()->Write("NativeCompiler", LogPanic, "ERROR: Failed to disassemble given code!");
    }
    cs_close(&handle);
}
