#find . -name "*.d" -type f -delete

include ../circle-stdlib/Config.mk

CIRCLEHOME = ../circle-stdlib/libs/circle
NEWLIBDIR = ../circle-stdlib/install/$(NEWLIB_ARCH)

OBJS    = 	main.o kernel.o \
			OS/Chrono/Chrono.o \
			OS/OS.o OS/OS_Tasks.o OS/OS_Strings.o OS/OS_Tasks_Code.o OS/Breakdown.o OS/OS_Tasks_Strings.o OS/OS_Tasks_Data.o \
			OS/OS_Tasks_GUI.o OS/OS_Tasks_Memory.o \
			Tasks/System/InputManager/InputManager.o \
			\
			Tasks/System/WindowManager/window/Window.o Tasks/System/WindowManager/lvglwindow/LVGLWindow.o Tasks/System/WindowManager/canvas/Canvas.o \
			Tasks/System/WindowManager/WindowManager.o Tasks/System/WindowManager/style/Style.o Tasks/System/WindowManager/lvgl/lvgl.o \
			\
			Tasks/System/IconBar/IconBar.o \
			Tasks/DARICWindow.o \
			Tasks/System/TasksWindow/TasksWindow.o \
			Tasks/System/Filer/Filer.o \
			Tasks/System/FontManager/FontManager.o \
			Tasks/System/Menu/Menu.o \
			Tasks/System/FileManager/FileManager.o \
			Tasks/Editor/Editor.o \
			Library/StringLib.o \
			\
			Compiler/Tokeniser/Tokeniser.o Compiler/Tokeniser/TokeniserNumber.o Compiler/Tokeniser/TokeniserString.o Compiler/Tokeniser/TokeniserSymbol.o \
			Compiler/Tokeniser/TokeniserText.o Compiler/Tokeniser/TokeniserPrinter.o \
			\
			Compiler/Parser/Parser.o Compiler/Parser/ParserExpression.o Compiler/Parser/Parser_DEF.o Compiler/Parser/Parser_IF.o \
			Compiler/Parser/Parser_PRINT.o Compiler/Parser/Parser_PROCCall.o \
			Compiler/Parser/Parser_CONST.o Compiler/Parser/Parser_SWAP.o Compiler/Parser/Parser_Assignment.o \
			Compiler/Parser/Parser_REPEAT.o Compiler/Parser/Parser_WHILE.o Compiler/Parser/Parser_FOR.o Compiler/Parser/Parser_CASE.o Compiler/Parser/Parser_DATA.o \
			Compiler/Parser/Parser_TYPE.o \
			\
			Compiler/IRCompiler/IRComparison.o Compiler/IRCompiler/IRCompiler.o Compiler/IRCompiler/IRIf.o Compiler/IRCompiler/IRLiteral.o \
			Compiler/IRCompiler/IRMaths.o Compiler/IRCompiler/IRPrinter.o Compiler/IRCompiler/IRBoolean.o Compiler/IRCompiler/IRConversion.o \
			Compiler/IRCompiler/IRVariables.o Compiler/IRCompiler/IRAssignment.o Compiler/IRCompiler/IRPrint.o \
			Compiler/IRCompiler/IRRepeat.o Compiler/IRCompiler/IRWhile.o Compiler/IRCompiler/IRFor.o Compiler/IRCompiler/IROptimiser.o \
			Compiler/IRCompiler/IRCase.o Compiler/IRCompiler/IRData.o Compiler/IRCompiler/IRDefs.o \
			\
			Compiler/NativeCompiler/Disassemble.o Compiler/NativeCompiler/NativeCompiler.o Compiler/NativeCompiler/NativeStack.o \
			OS/Library/LibPrint.o OS/Library/LibMaths.o OS/Library/LibString.o OS/Library/LibGeneral.o OS/Library/LibChrono.o \
			OS/Library/LibGraphics2D.o OS/Library/LibData.o OS/Library/LibKeyboardMouse.o OS/Library/LibRandom.o \
			\
			Compiler/Lightning/lightning.o Compiler/Lightning/jit_memory.o Compiler/Lightning/jit_disasm.o Compiler/Lightning/jit_size.o Compiler/Lightning/jit_note.o Compiler/Lightning/jit_print.o \
			Compiler/capstone/arch/AArch64/AArch64BaseInfo.o Compiler/capstone/arch/AArch64/AArch64Disassembler.o Compiler/capstone/arch/AArch64/AArch64InstPrinter.o Compiler/capstone/arch/AArch64/AArch64Mapping.o \
			Compiler/capstone/arch/AArch64/AArch64Module.o Compiler/capstone/cs.o Compiler/capstone/MCInst.o Compiler/capstone/MCInstrDesc.o Compiler/capstone/MCRegisterInfo.o Compiler/capstone/SStream.o Compiler/capstone/utils.o \

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I ../circle-stdlib/include -I Compiler/capstone/include
LIBS := "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" \
 	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
  	$(CIRCLEHOME)/lib/usb/libusb.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
 	$(CIRCLEHOME)/addon/lvgl/liblvgl.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
  	$(CIRCLEHOME)/lib/libcircle.a \
	$(CIRCLEHOME)/addon/wlan/hostap/wpa_supplicant/libwpa_supplicant.a \
	$(CIRCLEHOME)/addon/wlan/libwlan.a

CFLAGS += -I capstone -Wno-unused-function
CFLAGS += -D CAPSTONE_HAS_ARM64=1 -D CAPSTONE_USE_SYS_DYN_MEM=no

-include $(DEPS)

clean:
	@echo "  CLEAN " `pwd`
	find . -name '*.o' -exec rm -r {} \;
	find . -name '*.d' -exec rm -r {} \;
	@rm -f *.a *.elf *.lst *.img *.hex *.cir *.map *~ $(EXTRACLEAN)
