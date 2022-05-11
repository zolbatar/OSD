#include "NativeCompiler.h"
#include <capstone/capstone.h>
#include <capstone/platform.h>

void NativeCompiler::Disassemble(std::list<std::string>* disass)
{
	// Disassemble
	csh handle;
	cs_insn* insn;
	size_t count;
#ifndef CLION
	cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
#else
#ifdef __x86_64__
	cs_err err = cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
#else
	cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
#endif
#endif
	if (err!=CS_ERR_OK) {
		printf("Error: %d\n", err);
		return;
	}
	cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
	count = cs_disasm(handle, (uint8_t*)GetCurrentTask()->GetExec(), code_size, (uint64_t)_jit->code.ptr, 0, &insn);
	if (count>0) {
		size_t j;
		for (j = 0; j<count; j++) {
			char f[256];
			sprintf(f, "0x%" PRIx64 ":\t%s\t%s", insn[j].address, insn[j].mnemonic, insn[j].op_str);
			disass->push_back(std::string(f));
		}

		cs_free(insn, count);
	}
	else {
		printf("ERROR: Failed to disassemble given code!\n");
		return;
	}
	cs_close(&handle);
}
