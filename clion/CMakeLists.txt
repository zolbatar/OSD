cmake_minimum_required(VERSION 3.20)
project(OSD)

set(CMAKE_CXX_STANDARD 20)

include(conanbuildinfo.cmake)
conan_basic_setup()

include_directories("../capstone/include")

find_package(SDL2 REQUIRED)
link_directories("/Users/daryl/GitHub/lvgl/lib")
include_directories("/Users/daryl/GitHub/lvgl/src")

add_definitions(-DCLION -DHAVE_FFSL -DCAPSTONE_HAS_ARM64=1 -DCAPSTONE_HAS_ARM=1 -DCAPSTONE_HAS_X86=1)
if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    add_definitions(-D__x86_64__ -U__aarch64__ -U__i386__ -U__arm64__)
else()
    add_definitions(-D__aarch64__ -U__i386__ -U__x86_64__ -D__arm64__)
endif()
include_directories(../../JIT/lightning)
include_directories(../capstone)

add_executable(
        OSD main.cpp
        ../../OS/OS.cpp ../../OS/OS_Strings.cpp ../../OS/OS_Tasks.cpp
        ../../Tasks/DARICWindow.cpp ../../Tasks/WindowManager.cpp ../../Tasks/Desktop.cpp ../../Tasks/TasksWindow.cpp ../../Tasks/FontManager.cpp
        ../../GUI/Window.cpp ../../GUI/lvgl.cpp ../../GUI/Style.cpp
        ../../GUI/Controls/Control.cpp ../../GUI/Controls/Canvas.cpp
        ../../Parser/Parser.cpp ../../Parser/ParserExpression.cpp
        ../../Parser/Parser_IF.cpp ../../Parser/Parser_PRINT.cpp ../../Parser/Parser_DEF.cpp ../../Parser/Parser_PROCCall.cpp
        ../../Parser/Parser_CONST.cpp ../../Parser/Parser_Assignment.cpp ../../Parser/Parser_SWAP.cpp
        ../../Parser/Parser_REPEAT.cpp ../../Parser/Parser_WHILE.cpp ../../Parser/Parser_FOR.cpp ../../Parser/Parser_CASE.cpp
        ../../Parser/Parser_DATA.cpp ../../Parser/Parser_TYPE.cpp
        ../../Tokeniser/Tokeniser.cpp ../../Tokeniser/TokeniserNumber.cpp ../../Tokeniser/TokeniserPrinter.cpp ../../Tokeniser/TokeniserString.cpp
        ../../Tokeniser/TokeniserSymbol.cpp ../../Tokeniser/TokeniserText.cpp
        ../../Exception/DARICException.h
        ../../IRCompiler/IRCompiler.cpp ../../IRCompiler/IRPrinter.cpp ../../IRCompiler/IRComparison.cpp
        ../../IRCompiler/IRMaths.cpp ../../IRCompiler/IRLiteral.cpp ../../IRCompiler/IRIf.cpp ../../IRCompiler/IRAssignment.cpp
        ../../IRCompiler/IRPrint.cpp ../../IRCompiler/IRRepeat.cpp ../../IRCompiler/IRWhile.cpp ../../IRCompiler/IRFor.cpp
        ../../IRCompiler/IRConversion.cpp ../../IRCompiler/IRBoolean.cpp ../../IRCompiler/IRString.cpp ../../IRCompiler/IRVariables.cpp
        ../../IRCompiler/IROptimiser.cpp ../../IRCompiler/IRCase.cpp ../../IRCompiler/IRData.cpp ../../IRCompiler/IRDefs.cpp
        ../../NativeCompiler/NativeCompiler.cpp ../../NativeCompiler/NativeStack.cpp ../../NativeCompiler/LibPrint.cpp
        ../../NativeCompiler/Disassemble.cpp ../../NativeCompiler/LibMaths.cpp ../../NativeCompiler/LibGeneral.cpp
        ../../NativeCompiler/LibString.cpp ../../NativeCompiler/LibChrono.cpp ../../NativeCompiler/LibGraphics2D.cpp
        ../../NativeCompiler/LibData.cpp
        ../../Lightning/lightning_orig.c ../../Lightning/jit_memory.c ../../Lightning/jit_print.c
        ../../Lightning/jit_disasm.c ../../Lightning/jit_size.c ../../Lightning/jit_note.c
        ../../capstone/arch/AArch64/AArch64BaseInfo.c ../../capstone/arch/AArch64/AArch64Disassembler.c ../../capstone/arch/AArch64/AArch64InstPrinter.c
        ../../capstone/arch/AArch64/AArch64Mapping.c ../../capstone/arch/AArch64/AArch64Module.c
        ../../capstone/arch/X86/X86ATTInstPrinter.c ../../capstone/arch/X86/X86Disassembler.c ../../capstone/arch/X86/X86DisassemblerDecoder.c
        ../../capstone/arch/X86/X86IntelInstPrinter.c ../../capstone/arch/X86/X86Mapping.c ../../capstone/arch/X86/X86Module.c
        ../../capstone/arch/X86/X86InstPrinterCommon.c
        ../../capstone/cs.c ../../capstone/MCInst.c ../../capstone/MCInstrDesc.c ../../capstone/MCRegisterInfo.c
        ../../capstone/SStream.c ../../capstone/utils.c ../../capstone/arch/ARM/ARMDisassembler.c ../../capstone/arch/ARM/ARMInstPrinter.c
        ../../capstone/arch/ARM/ARMMapping.c ../../capstone/arch/ARM/ARMModule.c
        ../Input/Input.cpp ../Input/Input.h ../Chrono/Chrono.cpp ../Chrono/Chrono.h
        ../NativeCompiler/NativeCompiler.cpp ../NativeCompiler/NativeCompiler.h ../IRCompiler/IRConversion.cpp ../Parser/Parser_WHILE.cpp ../IRCompiler/IRWhile.cpp)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS} liblvgl.a)
