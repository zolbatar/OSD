#include "NativeCompiler.h"
#include <circle/logger.h>
#include <capstone/capstone.h>
#include <capstone/platform.h>
#include "../Parser/Parser.h"
#include "../OS/Breakdown.h"

NativeCompiler::NativeCompiler(bool optimise, jit_state_t *_jit, OSDTask *task)
    : optimise(optimise), _jit(_jit), task(task)
{
}

void NativeCompiler::IRToNative(std::list<IRInstruction> *ir_global, std::list<IRInstruction> *ir, bool debug)
{
    // Add all global sizes
    size_t globals_size = 0;
    for (auto &op : *ir_global)
    {
        switch (op.type)
        {
        case IROpcodes::VariableGlobalCreateInteger:
            globals_size += sizeof(int64_t);
            break;
        case IROpcodes::VariableGlobalCreateFloat:
            globals_size += sizeof(double);
            break;
        case IROpcodes::VariableGlobalCreateString:
            globals_size += sizeof(int64_t);
            break;
        default:
            // Don't care
            break;
        }
    }

    // Global code
    jit_prolog();
    StackInit();
    jit_movr(JIT_V1, JIT_FP);
    IRToNativeSection(ir_global, debug, true);
    jit_ret();
    jit_epilog();

    /*	for (auto& v: global_variables)
            printf("Global alloc: %d\n", v);*/

    // PROCs etc
    IRToNativeSection(ir, debug, false);

    // Optimise stuff?
    if (optimise)
    {
        // jit_print();

#ifdef VERBOSE_COMPILE
        int count = 0;
        for (auto node = _jitc->head; node; node = node->next)
        {
            count++;
        }
        CLogger::Get()->Write("Native Compiler", LogDebug, "Count before native opt: %d", count);
#endif

        jit_node_t *previous = NULL;
        for (auto node = _jitc->head; node; node = node->next)
        {

            // No gap, add/sub reg
            if (node->code == jit_code_addi && node->u.w == node->v.w)
            {
                auto next = node->next;
                if (next->code == jit_code_subi && next->u.w == next->v.w && node->u.w == next->v.w &&
                    next->w.w == node->w.w)
                {
                    previous->next = next->next;
                    node = next;
                }
            }

            // With prepare in the middle, add/sub reg
            if (node->code == jit_code_addi && node->u.w == node->v.w)
            {
                auto next = node->next;
                auto nextnext = next->next;
                if (next->code == jit_code_prepare && nextnext->code == jit_code_subi &&
                    nextnext->u.w == nextnext->v.w && node->u.w == nextnext->v.w && nextnext->w.w == node->w.w)
                {
                    previous->next = nextnext->next;
                    node = nextnext;
                }
            }
            previous = node;
        }

#ifdef VERBOSE_COMPILE
        count = 0;
        for (auto node = _jitc->head; node; node = node->next)
        {
            count++;
        }
        CLogger::Get()->Write("Native Compiler", LogDebug, "Count after native opt: %d", count);
#endif
        // jit_print();
    }

    // Code & data size
    jit_realize();
    if (!_jitc->realize)
    {
        CLogger::Get()->Write("Native Compiler", LogPanic, "Failed to realise");
    }
    task->CreateCode(_jit->code.length);
    jit_set_code(task->GetCode(), task->GetCodeSize());
    jit_set_data(NULL, 0, JIT_DISABLE_NOTE | JIT_DISABLE_NOTE);
    start exec;
    try
    {
        exec = jit_emit_void();
        if (exec == NULL)
        {
            CLogger::Get()->Write("Native Compiler", LogPanic, "Code generation failed");
        }
    }
    catch (const std::exception &ex)
    {
        CLogger::Get()->Write("Native Compiler", LogPanic, "Error emitting: %s", ex.what());
    }
    task->SetStart(exec);

    // Dump line mappings
    if (debug)
        Breakdown::ProcessLineMappings(_jit);

    // Size?
    jit_word_t code_size;
    jit_get_code(&code_size);
#ifdef VERBOSE_COMPILE
    CLogger::Get()->Write("Native Compiler", LogDebug, "Code size: %ld bytes", code_size);
#endif
}

int NativeCompiler::IdxForVar(IRInstruction &op, int64_t index)
{
    if (index >= 0)
    {
        size_t ii = index;
        if (ii > local_variables.size())
            CLogger::Get()->Write("Native Compiler", LogPanic, "Invalid local: %d/%d at %d", ii, local_variables.size(),
                                  op.line_number);
        return local_variables[index];
    }
    else
    {
        size_t ii = (-index) - 1;
        if (ii > global_variables.size())
            CLogger::Get()->Write("Native Compiler", LogPanic, "Invalid global: %d/%d/%d at %d", ii, index,
                                  global_variables.size(), op.line_number);
        return global_variables[(-index) - 1];
    }
}

jit_reg_t NativeCompiler::RegForVar(int64_t index)
{
    if (index >= 0)
        return JIT_FP;
    else
        return JIT_V1;
}

void call_STACKCHECK(int64_t sp, int64_t line_number, uint64_t fp)
{
    if (sp > 0)
    {
        printf("Stack overflow. Quitting.");
        exit(0);
    }
    else
    {
        // Show local var space
        for (auto i = 0; i < 8; i++)
        {
            uint64_t *p = (uint64_t *)(fp - 2064 - (i * sizeof(int64_t)));
            int64_t v = *p;
            printf("[%p] %" PRId64 "\n", p, v);
        }
        printf("SP: %" PRId64 "=%" PRId64 "\n", line_number, sp);
    }
}

void NativeCompiler::IRToNativeSection(std::list<IRInstruction> *ir, bool debug, bool global)
{
    for (auto &op : *ir)
    {
        if (global)
            Breakdown::InsertLineMappingGlobal(op.line_number, jit_indirect());
        else
            Breakdown::InsertLineMapping(op.line_number, jit_indirect());

        switch (op.type)
        {
        case IROpcodes::End:
            jit_prepare();
            jit_finishi((jit_pointer_t)call_END);
            break;
        case IROpcodes::Yield:
            jit_prepare();
            jit_finishi((jit_pointer_t)call_YIELD);
            break;

            // Optimised opcodes
        case IROpcodes::NOP:
            break;
        case IROpcodes::StackCheck:
            jit_prepare();
            jit_pushargr(JIT_V0);
            jit_movi(JIT_R0, op.index);
            jit_pushargr(JIT_R0);
            jit_pushargr(JIT_FP);
            jit_finishi((jit_pointer_t)call_STACKCHECK);
            break;

        case IROpcodes::SwapIntegerOperands:
            jit_movr(JIT_R2, JIT_R0);
            jit_movr(JIT_R0, JIT_R1);
            jit_movr(JIT_R1, JIT_R2);
            break;
        case IROpcodes::SwapFloatOperands:
            SF(jit_movr)(JIT_F2, JIT_F0);
            SF(jit_movr)(JIT_F0, JIT_F1);
            SF(jit_movr)(JIT_F1, JIT_F2);
            break;
        case IROpcodes::IntegerOperand1To2:
            jit_movr(JIT_R1, JIT_R0);
            break;
        case IROpcodes::IntegerOperand2To1:
            jit_movr(JIT_R0, JIT_R1);
            break;
        case IROpcodes::FloatOperand1To2:
            SF(jit_movr)(JIT_F1, JIT_F0);
            break;
        case IROpcodes::FloatOperand2To1:
            SF(jit_movr)(JIT_F0, JIT_F1);
            break;
        case IROpcodes::LoadIntegerOperands:
            jit_ldxi_l(JIT_R1, RegForVar(op.index), IdxForVar(op, op.index));
            jit_ldxi_l(JIT_R0, RegForVar(op.index2), IdxForVar(op, op.index2));
            break;
        case IROpcodes::LoadFloatOperands:
            SF(jit_ldxi)(JIT_F1, RegForVar(op.index), IdxForVar(op, op.index));
            SF(jit_ldxi)(JIT_F0, RegForVar(op.index2), IdxForVar(op, op.index2));
            break;

            // Variables
        case IROpcodes::VariableLocalCreateInteger:
            local_variables.push_back(jit_allocai(sizeof(int64_t)));
            break;
        case IROpcodes::VariableLocalCreateFloat:
            local_variables.push_back(jit_allocai(sizeof(double)));
            break;
        case IROpcodes::VariableLocalCreateString:
            local_variables.push_back(jit_allocai(sizeof(int64_t)));
            local_string_variables.push_back(op.index);
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), 0);
            break;
        case IROpcodes::VariableLocalCreateType:
            local_variables.push_back(jit_allocai(op.iv));
            break;
        case IROpcodes::VariableLocalCreateTypeString: {

            // Clear
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_movi(JIT_R0, 0);
            jit_stxr_l(JIT_R2, RegForVar(op.index), JIT_R0);

            // Save to clear
            TypeOffset to;
            to.index = op.index;
            to.offset = op.iv;
            local_string_variables_types.push_back(std::move(to));
            break;
        }
        case IROpcodes::VariableGlobalCreateInteger:
            global_variables.push_back(jit_allocai(sizeof(int64_t)));
            break;
        case IROpcodes::VariableGlobalCreateFloat:
            global_variables.push_back(jit_allocai(sizeof(double)));
            break;
        case IROpcodes::VariableGlobalCreateString:
            global_variables.push_back(jit_allocai(sizeof(int64_t)));
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), 0);
            break;
        case IROpcodes::VariableGlobalCreateType:
            global_variables.push_back(jit_allocai(op.iv));
            break;
        case IROpcodes::VariableGlobalCreateTypeString: {
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_movi(JIT_R0, 0);
            jit_stxr_l(JIT_R2, RegForVar(op.index), 0);
            break;
        }
        case IROpcodes::VariableLoadInteger:
            jit_ldxi_l(JIT_R0, RegForVar(op.index), IdxForVar(op, op.index));
            break;
        case IROpcodes::VariableLoadFloat:
            SF(jit_ldxi)(JIT_F0, RegForVar(op.index), IdxForVar(op, op.index));
            break;
        case IROpcodes::VariableLoadString:
            jit_ldxi_l(JIT_R0, RegForVar(op.index), IdxForVar(op, op.index));
            break;
        case IROpcodes::VariableStoreInteger:
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);
            break;
        case IROpcodes::VariableStoreFloat:
            SF(jit_stxi)(IdxForVar(op, op.index), RegForVar(op.index), JIT_F0);
            break;
        case IROpcodes::VariableStoreString:
            jit_movr(JIT_V2, JIT_R0);

            // Free old value
            jit_ldxi_l(JIT_R0, RegForVar(op.index), IdxForVar(op, op.index));
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_freepermanent);

            // Store
            jit_movr(JIT_R0, JIT_V2);
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);

            // Make permanent
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_makepermanent);
            break;

            // Offsets
        case IROpcodes::VariableLoadIntegerWithOffset:
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_ldxr_l(JIT_R0, RegForVar(op.index), JIT_R2);
            break;
        case IROpcodes::VariableLoadFloatWithOffset:
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            SF(jit_ldxr)(JIT_F0, RegForVar(op.index), JIT_R2);
            break;
        case IROpcodes::VariableLoadStringWithOffset:
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_ldxr_l(JIT_R0, RegForVar(op.index), JIT_R2);
            break;
        case IROpcodes::VariableStoreIntegerWithOffset:
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_stxr_l(JIT_R2, RegForVar(op.index), JIT_R0);
            break;
        case IROpcodes::VariableStoreFloatWithOffset:
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            SF(jit_stxr)(JIT_R2, RegForVar(op.index), JIT_F0);
            break;
        case IROpcodes::VariableStoreStringWithOffset:
            jit_movr(JIT_V2, JIT_R0);

            // Free old value
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_ldxr_l(JIT_R0, RegForVar(op.index), JIT_R2);
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_freepermanent);

            // Store
            jit_movr(JIT_R0, JIT_V2);
            jit_movi(JIT_R2, IdxForVar(op, op.index));
            jit_addi(JIT_R2, JIT_R2, op.iv);
            jit_stxr_l(JIT_R2, RegForVar(op.index), JIT_R0);

            // Make permanent
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_makepermanent);
            break;

            // Swap
        case IROpcodes::VariableSwapSourceInteger:
            swap_source = &op;
            break;
        case IROpcodes::VariableSwapDestinationInteger:
            jit_ldxi_l(JIT_R0, RegForVar(swap_source->index), IdxForVar(op, swap_source->index));
            jit_ldxi_l(JIT_R1, RegForVar(op.index), IdxForVar(op, op.index));
            jit_stxi_l(IdxForVar(op, swap_source->index), RegForVar(swap_source->index), JIT_R1);
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);
            break;
        case IROpcodes::VariableSwapSourceFloat:
            swap_source = &op;
            break;
        case IROpcodes::VariableSwapDestinationFloat:
            SF(jit_ldxi)(JIT_F0, RegForVar(swap_source->index), IdxForVar(op, swap_source->index));
            SF(jit_ldxi)(JIT_F1, RegForVar(op.index), IdxForVar(op, op.index));
            SF(jit_stxi)(IdxForVar(op, swap_source->index), RegForVar(swap_source->index), JIT_F1);
            SF(jit_stxi)(IdxForVar(op, op.index), RegForVar(op.index), JIT_F0);
            break;
        case IROpcodes::VariableSwapSourceString:
            swap_source = &op;
            break;
        case IROpcodes::VariableSwapDestinationString:
            jit_ldxi_l(JIT_R0, RegForVar(swap_source->index), IdxForVar(op, swap_source->index));
            jit_ldxi_l(JIT_R1, RegForVar(op.index), IdxForVar(op, op.index));
            jit_stxi_l(IdxForVar(op, swap_source->index), RegForVar(swap_source->index), JIT_R1);
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);
            break;

            // Literals
        case IROpcodes::LiteralInteger:
            jit_movi(JIT_R0, op.iv);
            break;
        case IROpcodes::LiteralFloat:
            SF(jit_movi)(JIT_F0, op.rv);
            break;
        case IROpcodes::LiteralString: {
            auto idx = OS_Strings_CreatePermanent(op.sv);
            jit_movi(JIT_R0, idx);
            break;
        }

            // Stack
        case IROpcodes::StackPopIntOperand1:
            StackPopInt(JIT_R0);
            break;
        case IROpcodes::StackPopIntOperand2:
            StackPopInt(JIT_R1);
            break;
        case IROpcodes::StackPushIntOperand1:
            StackPushInt(JIT_R0);
            break;
        case IROpcodes::StackPushIntOperand2:
            StackPushInt(JIT_R1);
            break;
        case IROpcodes::StackPopFloatOperand1:
            StackPopFloat(JIT_F0);
            break;
        case IROpcodes::StackPopFloatOperand2:
            StackPopFloat(JIT_F1);
            break;
        case IROpcodes::StackPushFloatOperand1:
            StackPushFloat(JIT_F0);
            break;
        case IROpcodes::StackPushFloatOperand2:
            StackPushFloat(JIT_F1);
            break;
        case IROpcodes::StackPopStringOperand1:
            StackPopString(JIT_R0);
            break;
        case IROpcodes::StackPopStringOperand2:
            StackPopString(JIT_R1);
            break;
        case IROpcodes::StackPushStringOperand1:
            StackPushString(JIT_R0);
            break;

            // Conversion
        case IROpcodes::ConvertOperand1StringToInt1:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_stringToInt);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::ConvertOperand1StringToFloat1:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_stringToFloat);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::ConvertOperand1FloatToString1:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_STRING_realtostring);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::ConvertOperand1IntToString1:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_inttostring);
            jit_retval_l(JIT_R0);
            break;

        case IROpcodes::ConvertOperand1IntToFloat1:
            SF(jit_extr)(JIT_F0, JIT_R0);
            break;
        case IROpcodes::ConvertOperand2IntToFloat2:
            SF(jit_extr)(JIT_F1, JIT_R1);
            break;
        case IROpcodes::ConvertOperand1FloatToInt1:
            SF_l(jit_truncr)(JIT_R0, JIT_F0);
            break;
        case IROpcodes::ConvertOperand2FloatToInt2:
            SF_l(jit_truncr)(JIT_R1, JIT_F1);
            break;

            // Maths
        case IROpcodes::MathsUnaryMinusInt:
            jit_negr(JIT_R0, JIT_R0);
            break;
        case IROpcodes::MathsUnaryMinusFloat:
            jit_negr_d(JIT_F0, JIT_F0);
            break;
        case IROpcodes::MathsAddString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_add);
            jit_retval(JIT_R0);
            break;
        case IROpcodes::MathsAddInt:
            jit_addr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsSubtractInt:
            jit_subr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsMultiplyInt:
            jit_mulr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsAddFloat:
            SF(jit_addr)(JIT_F0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::MathsSubtractFloat:
            SF(jit_subr)(JIT_F0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::MathsMultiplyFloat:
            SF(jit_mulr)(JIT_F0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::MathsDivide:
            SF(jit_divr)(JIT_F0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::MathsShiftLeft:
            jit_lshr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsShiftRight:
            jit_rshr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsPower:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            SF(jit_pushargr)(JIT_F1);
            jit_finishi((jit_pointer_t)call_MATHS_power);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsMod:
            jit_remr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsDiv:
            jit_divr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::MathsAbsInt:
            SF(jit_extr)(JIT_F0, JIT_R0);
            SF(jit_absr)(JIT_F0, JIT_F0);
            SF(jit_truncr)(JIT_R0, JIT_F0);
            break;
        case IROpcodes::MathsAbsFloat:
            SF(jit_absr)(JIT_F0, JIT_F0);
            break;
        case IROpcodes::MathsSqr:
            SF(jit_sqrtr)(JIT_F0, JIT_F0);
            break;
        case IROpcodes::MathsLn:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_ln);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsLog:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_log);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsExp:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_exp);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsSin:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_sin);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsAsn:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_asn);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsCos:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_cos);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsAcs:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_acs);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsTan:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_tan);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsAtn:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_atn);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsDeg:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_deg);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsRad:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_rad);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::MathsSgn:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_MATHS_sgn);
            jit_retval_l(JIT_R0);
            break;

            // Boolean
        case IROpcodes::BooleanNot:
            jit_comr(JIT_R0, JIT_R0);
            break;
        case IROpcodes::BooleanAnd:
            jit_andr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::BooleanOr:
            jit_orr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::BooleanEor:
            jit_xorr(JIT_R0, JIT_R1, JIT_R0);
            break;

            // Comparison
        case IROpcodes::CompareEqualInt:
            jit_eqr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareEqualFloat:
            SF(jit_eqr)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareEqualString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_equal);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::CompareNotEqualInt:
            jit_ner(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareNotEqualFloat:
            SF(jit_ner)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareNotEqualString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_notequal);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::CompareLessInt:
            jit_ltr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareLessFloat:
            SF(jit_ltr)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareLessString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_less);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::CompareLessEqualInt:
            jit_ler(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareLessEqualFloat:
            SF(jit_ler)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareLessEqualString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_lessequal);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::CompareGreaterInt:
            jit_gtr(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareGreaterFloat:
            SF(jit_gtr)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareGreaterString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_greater);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::CompareGreaterEqualInt:
            jit_ger(JIT_R0, JIT_R1, JIT_R0);
            break;
        case IROpcodes::CompareGreaterEqualFloat:
            SF(jit_ger)(JIT_R0, JIT_F1, JIT_F0);
            break;
        case IROpcodes::CompareGreaterEqualString:
            jit_prepare();
            jit_pushargr(JIT_R1);
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_STRING_greaterequal);
            jit_retval_l(JIT_R0);
            break;

            // Jumps
        case IROpcodes::JumpCreate:
            jump_labels.insert(std::make_pair(op.index, jit_label()));
            break;
        case IROpcodes::JumpOnConditionTrue:
            jit_patch_at(jit_beqi(JIT_R0, 1), jump_labels.find(op.index)->second);
            break;
        case IROpcodes::JumpOnConditionFalse:
            jit_patch_at(jit_beqi(JIT_R0, 0), jump_labels.find(op.index)->second);
            break;
        case IROpcodes::JumpCreateForward:
            // Create forward label
            jump_labels.insert(std::make_pair(op.index, jit_forward()));
            jump_patches.insert(std::make_pair(op.index, std::list<jit_node_t *>()));
            break;
        case IROpcodes::JumpDestination: {
            // Do all patches
            auto label = jump_labels.find(op.index)->second;
            for (auto &p : jump_patches.find(op.index)->second)
                jit_patch_at(p, label);
            jit_link(label);
            break;
        }
        case IROpcodes::Jump:
            jit_patch_at(jit_jmpi(), jump_labels.find(op.index)->second);
            break;
        case IROpcodes::JumpForward:
            jump_patches.find(op.index)->second.push_back(jit_jmpi());
            break;
        case IROpcodes::JumpOnConditionTrueForward:
            jump_patches.find(op.index)->second.push_back(jit_beqi(JIT_R0, 1));
            break;
        case IROpcodes::JumpOnConditionFalseForward:
            jump_patches.find(op.index)->second.push_back(jit_beqi(JIT_R0, 0));
            break;

            // PROC
        case IROpcodes::PROCStart: {
            forward_ret = jit_forward();
            ret_patches.clear();
            local_variables.clear();
            local_string_variables.clear();
            local_string_variables_types.clear();
            auto call = jit_label();
            proc_labels.insert(std::make_pair(op.index, call));
            if (proc_patches.find(op.index) != proc_patches.end())
                for (auto &p : proc_patches.find(op.index)->second)
                    jit_patch_at(p, call);
            jit_link(call);
            jit_prolog();

            StackInit();
            break;
        }
        case IROpcodes::PROCEnd:
            jit_epilog();
            break;
        case IROpcodes::ParameterInt:
            jit_getarg_l(JIT_R0, jit_arg());
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);
            break;
        case IROpcodes::ParameterFloat:
            SF(jit_getarg)(JIT_F0, SF(jit_arg)());
            SF(jit_stxi)(IdxForVar(op, op.index), RegForVar(op.index), JIT_F0);
            break;
        case IROpcodes::ParameterString:
            jit_getarg_l(JIT_R0, jit_arg());
            jit_stxi_l(IdxForVar(op, op.index), RegForVar(op.index), JIT_R0);
            break;
        case IROpcodes::ReturnNone:
            if (op.index == 1)
            {
                for (auto &p : ret_patches)
                    jit_patch_at(p, forward_ret);
                jit_link(forward_ret);
                jit_ret();
            }
            else
                ret_patches.push_back(jit_jmpi());
            break;
        case IROpcodes::ReturnInteger:
            if (op.index == 1)
            {
                for (auto &p : ret_patches)
                    jit_patch_at(p, forward_ret);
                jit_link(forward_ret);
                jit_retr(JIT_R0);
            }
            else
                ret_patches.push_back(jit_jmpi());
            break;
        case IROpcodes::ReturnFloat:
            if (op.index == 1)
            {
                for (auto &p : ret_patches)
                    jit_patch_at(p, forward_ret);
                jit_link(forward_ret);
                SF(jit_retr)(JIT_F0);
            }
            else
                ret_patches.push_back(jit_jmpi());
            break;
        case IROpcodes::ReturnString:
            if (op.index == 1)
            {
                for (auto &p : ret_patches)
                    jit_patch_at(p, forward_ret);
                jit_link(forward_ret);
                jit_retr(JIT_R0);
            }
            else
                ret_patches.push_back(jit_jmpi());
            break;

        case IROpcodes::PROCCall: {
            jit_prepare();
            ConstructArguments();
            auto call = jit_finishi(NULL);
            auto pl = proc_labels.find(op.index);
            if (pl == proc_labels.end())
            {
                // Forward lookup
                if (proc_patches.find(op.index) == proc_patches.end())
                    proc_patches.insert(std::make_pair(op.index, std::list<jit_node_t *>()));
                proc_patches.find(op.index)->second.push_back(call);
            }
            else
            {
                jit_patch_at(call, proc_labels.find(op.index)->second);
            }

            // Release any local string variables
            for (auto &local : local_string_variables)
            {
                jit_ldxi_l(JIT_R0, RegForVar(local), IdxForVar(op, local));
                jit_prepare();
                jit_pushargr(JIT_R0);
                jit_finishi((jit_pointer_t)call_STRING_freepermanent);
            }
            for (auto &local : local_string_variables_types)
            {
                jit_movi(JIT_R2, IdxForVar(op, local.index));
                jit_addi(JIT_R2, JIT_R2, local.offset);
                jit_ldxr_l(JIT_R0, RegForVar(local.index), JIT_R2);
                jit_prepare();
                jit_pushargr(JIT_R0);
                jit_finishi((jit_pointer_t)call_STRING_freepermanent);
            }

            if (!returns.empty())
            {
                auto type = returns.top();
                returns.pop();
                switch (type)
                {
                case ValueType::Integer:
                    jit_retval_l(JIT_R0);
                    StackPushInt(JIT_R0);
                    break;
                case ValueType::Float:
                    SF(jit_retval)(JIT_F0);
                    StackPushFloat(JIT_F0);
                    break;
                case ValueType::String:
                    jit_retval_l(JIT_R0);
                    StackPushString(JIT_R0);
                    break;
                }
            }
            break;
        }

            // Data
        case IROpcodes::DataInteger:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_DATA_integer);
            break;
        case IROpcodes::DataFloat:
            jit_prepare();
            SF(jit_pushargr)(JIT_F0);
            jit_finishi((jit_pointer_t)call_DATA_float);
            break;
        case IROpcodes::DataString:
            jit_movr(JIT_V2, JIT_R0);
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_DATA_string);
            break;
        case IROpcodes::DataLabel:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_DATA_label);
            break;
        case IROpcodes::Restore:
            jit_prepare();
            jit_pushargr(JIT_R0);
            jit_finishi((jit_pointer_t)call_DATA_restore);
            break;
        case IROpcodes::ReadInteger:
            jit_prepare();
            jit_finishi((jit_pointer_t)call_READ_integer);
            jit_retval_l(JIT_R0);
            break;
        case IROpcodes::ReadFloat:
            jit_prepare();
            jit_finishi((jit_pointer_t)call_READ_float);
            SF(jit_retval)(JIT_F0);
            break;
        case IROpcodes::ReadString:
            jit_prepare();
            jit_finishi((jit_pointer_t)call_READ_string);
            jit_retval_l(JIT_R0);
            break;

            // Calling OS functions
        case IROpcodes::ArgumentInteger:
            arguments.push(ValueType::Integer);
            break;
        case IROpcodes::ArgumentFloat:
            arguments.push(ValueType::Float);
            break;
        case IROpcodes::ArgumentString:
            arguments.push(ValueType::String);
            break;
        case IROpcodes::StackReturnIntArgument:
            returns.push(ValueType::Integer);
            break;
        case IROpcodes::StackReturnFloatArgument:
            returns.push(ValueType::Float);
            break;
        case IROpcodes::StackReturnStringArgument:
            returns.push(ValueType::String);
            break;
        case IROpcodes::CallFunc: {

            // Lookup the actual address
            void *func = NULL;
            if (op.sv == "SPC")
                func = (void *)&call_PRINT_SPC;
            else if (op.sv == "TAB")
                func = (void *)&call_PRINT_TAB;
            else if (op.sv == "Tabbed On")
                func = (void *)&call_PRINT_Tabbed;
            else if (op.sv == "Tabbed Off")
                func = (void *)&call_PRINT_TabbedOff;
            else if (op.sv == "NL")
                func = (void *)&call_PRINT_NL;
            else if (op.sv == "PRINT integer")
                func = (void *)&call_PRINT_integer;
            else if (op.sv == "PRINT real")
                func = (void *)&call_PRINT_real;
            else if (op.sv == "PRINT string")
                func = (void *)&call_PRINT_string;
            else if (op.sv == "TIME")
                func = (void *)&call_TIME;
            else if (op.sv == "TIME$")
                func = (void *)&call_TIMES;
            else
                func = Parser::GetAddressForFunc(op.tt);

            jit_prepare();
            ConstructArguments();
            jit_finishi((jit_pointer_t)func);
            if (!returns.empty())
            {
                auto type = returns.top();
                returns.pop();
                switch (type)
                {
                case ValueType::Integer:
                    jit_retval_l(JIT_R0);
                    StackPushInt(JIT_R0);
                    break;
                case ValueType::Float:
                    SF(jit_retval)(JIT_F0);
                    StackPushFloat(JIT_F0);
                    break;
                case ValueType::String:
                    jit_retval_l(JIT_R0);
                    StackPushString(JIT_R0);
                    break;
                }
            }
            break;
        }

        default:
            assert(false);
        }
    }
}

void NativeCompiler::ConstructArguments()
{
    while (!arguments.empty())
    {
        auto a = arguments.top();
        arguments.pop();
        switch (a)
        {
        case ValueType::Integer:
            StackPopInt(JIT_R0);
            jit_pushargr(JIT_R0);
            break;
        case ValueType::Float:
            StackPopFloat(JIT_F0);
            SF(jit_pushargr)(JIT_F0);
            break;
        case ValueType::String:
            StackPopString(JIT_R0);
            jit_pushargr(JIT_R0);
            break;
        }
    }
}
