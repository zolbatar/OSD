#include "NativeCompiler.h"

void NativeCompiler::StackInit() {
    jit_movi(JIT_V0, jit_allocai(STACK_SIZE));
}

void NativeCompiler::StackPushInt(int reg) {
    jit_stxr_l(JIT_V0, JIT_FP, reg);
    jit_addi(JIT_V0, JIT_V0, sizeof(int64_t));
}

void NativeCompiler::StackPopInt(int reg) {
    jit_subi(JIT_V0, JIT_V0, sizeof(int64_t));
    jit_ldxr_l(reg, JIT_FP, JIT_V0);
}

void NativeCompiler::StackPushFloat(int reg) {
    SF(jit_stxr)(JIT_V0, JIT_FP, reg);
    jit_addi(JIT_V0, JIT_V0, sizeof(double));
}

void NativeCompiler::StackPopFloat(int reg) {
    jit_subi(JIT_V0, JIT_V0, sizeof(double));
    SF(jit_ldxr)(reg, JIT_FP, JIT_V0);
}

void NativeCompiler::StackPushString(int reg) {
    jit_stxr_l(JIT_V0, JIT_FP, reg);
    jit_addi(JIT_V0, JIT_V0, sizeof(int64_t));
}

void NativeCompiler::StackPopString(int reg) {
    jit_subi(JIT_V0, JIT_V0, sizeof(int64_t));
    jit_ldxr_l(reg, JIT_FP, JIT_V0);
}

