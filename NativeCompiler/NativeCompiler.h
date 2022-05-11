#pragma once
#include <list>
#include <string>
#include "../IRCompiler/IRCompiler.h"
#include <map>
#include "../OS/OS.h"
#include "Functions.h"

extern "C"
{
#include "../Lightning/lightning.h"
#include "../Lightning/jit_private.h"
}

#ifdef __aarch64__
#define SF(TYPE) TYPE##_d
#define SF_l(TYPE) TYPE##_d_l
#else
#define SF(TYPE) TYPE##_d
#define SF_l(TYPE) TYPE##_d_l
#endif

const int STACK_SIZE = 2048;
const int FRAME_SIZE = 4096;

class NativeCompiler {
public:
	NativeCompiler(bool optimise);
	~NativeCompiler();
	void IRToNative(std::list<IRInstruction>* ir_global, std::list<IRInstruction>* ir);
	void Disassemble(std::list<std::string>* disass);

private:
	bool optimise;
	void IRToNativeSection(std::list<IRInstruction>* ir);
	void StackInit();
	void StackPeekInt(int reg);
	void StackPushInt(int reg);
	void StackPopInt(int reg);
	void StackPushFloat(int reg);
	void StackPopFloat(int reg);
	void StackPushString(int reg);
	void StackPopString(int reg);
	void ConstructArguments();
	int IdxForVar(int64_t index);
	jit_reg_t RegForVar(int64_t index);

	jit_node_t* forward_ret;
	std::list<jit_node_t*> ret_patches;
	std::map<int64_t, jit_node_t*> proc_labels;
	std::map<size_t, std::list<jit_node_t*>> proc_patches;
	std::map<size_t, jit_node_t*> jump_labels;
	std::map<size_t, std::list<jit_node_t*>> jump_patches;
	std::vector<int> local_variables;
	std::vector<int> global_variables;
	std::stack<ValueType> arguments;
	std::stack<ValueType> returns;
	IRInstruction* swap_source;
	jit_state_t* _jit;
	jit_word_t code_size;
};