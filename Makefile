#find . -name "*.d" -type f -delete

include ../circle-stdlib/Config.mk

CIRCLEHOME = ../circle-stdlib/libs/circle
NEWLIBDIR = ../circle-stdlib/install/$(NEWLIB_ARCH)

OBJS    = 	main.o kernel.o \
			Chrono/Chrono.o \
			OS/OS.o OS/OS_Tasks.o OS/OS_Strings.o OS/OS_Tasks_Compile.o OS/Breakdown.o \
			Tasks/System/InputManager/InputManager.o \
			GUI/Window/Window.o GUI/Window/LVGLWindow.o GUI/Controls/Control.o GUI/Controls/Canvas.o \
			Tasks/System/WindowManager/WindowManager.o Tasks/System/WindowManager/Style.o Tasks/System/IconBar/IconBar.o \
			Tasks/DARICWindow.o Tasks/System/TasksWindow/TasksWindow.o Tasks/System/Filer/Filer.o \
			Tasks/System/FontManager/FontManager.o Tasks/System/Menu/Menu.o \
			Tasks/System/FileManager/FileManager.o \
			Tasks/Editor/Editor.o \
			GUI/lvgl.o  \
			Library/StringLib.o \
			\
			Tokeniser/Tokeniser.o Tokeniser/TokeniserNumber.o Tokeniser/TokeniserString.o Tokeniser/TokeniserSymbol.o \
			Tokeniser/TokeniserText.o Tokeniser/TokeniserPrinter.o \
			\
			Parser/Parser.o Parser/ParserExpression.o Parser/Parser_DEF.o Parser/Parser_IF.o Parser/Parser_PRINT.o Parser/Parser_PROCCall.o \
			Parser/Parser_CONST.o Parser/Parser_SWAP.o Parser/Parser_Assignment.o \
			Parser/Parser_REPEAT.o Parser/Parser_WHILE.o Parser/Parser_FOR.o Parser/Parser_CASE.o Parser/Parser_DATA.o \
			Parser/Parser_TYPE.o \
			\
			IRCompiler/IRComparison.o IRCompiler/IRCompiler.o IRCompiler/IRIf.o IRCompiler/IRLiteral.o \
			IRCompiler/IRMaths.o IRCompiler/IRPrinter.o IRCompiler/IRBoolean.o IRCompiler/IRConversion.o IRCompiler/IRString.o \
			IRCompiler/IRVariables.o IRCompiler/IRAssignment.o IRCompiler/IRPrint.o \
			IRCompiler/IRRepeat.o IRCompiler/IRWhile.o IRCompiler/IRFor.o IRCompiler/IROptimiser.o \
			IRCompiler/IRCase.o IRCompiler/IRData.o IRCompiler/IRDefs.o \
			\
			NativeCompiler/Disassemble.o NativeCompiler/LibPrint.o NativeCompiler/NativeCompiler.o NativeCompiler/NativeStack.o \
			NativeCompiler/LibMaths.o NativeCompiler/LibString.o NativeCompiler/LibGeneral.o NativeCompiler/LibChrono.o \
			NativeCompiler/LibGraphics2D.o NativeCompiler/LibData.o NativeCompiler/LibKeyboardMouse.o NativeCompiler/LibRandom.o \
			\
			Lightning/lightning.o Lightning/jit_memory.o Lightning/jit_disasm.o Lightning/jit_size.o Lightning/jit_note.o Lightning/jit_print.o \
			capstone/arch/AArch64/AArch64BaseInfo.o capstone/arch/AArch64/AArch64Disassembler.o capstone/arch/AArch64/AArch64InstPrinter.o capstone/arch/AArch64/AArch64Mapping.o \
			capstone/arch/AArch64/AArch64Module.o capstone/cs.o capstone/MCInst.o capstone/MCInstrDesc.o capstone/MCRegisterInfo.o capstone/SStream.o capstone/utils.o \

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -I ../circle-stdlib/include -I capstone/include
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
