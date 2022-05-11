#include "Editor.h"
#include "../Console/Console.h"
#include "../Input/Input.h"
#include "../Chrono/Chrono.h"
#include <cstring>

#ifndef CLION
#include <circle/memory.h>
extern CMemorySystem *memory;
#endif

#include <set>
#include <sstream>
#include <fstream>
#include "../Applications/Tester.h"
#include "../Applications/Mandelbrot.h"
#include "../Applications/Clock.h"

extern Input* input;
#ifdef CLION
extern UISDL ui;
#endif
std::list<DARICException> errors;
std::set<uint32_t> error_lines;

Editor::Editor()
{
//	std::stringstream ss(DARIC_mandelbrot);
//	std::stringstream ss(DARIC_tester);
	std::stringstream ss(DARIC_clock);
	std::string item;
	while (std::getline(ss, item, '\n')) {
		lines.push_back(item);
	}
}

void Editor::Run()
{
	Compile(true, false, false);
}

void Editor::Enter()
{
	Console_Clear();
	Header();
	WriteContent();

	while (1) {
		Console_Flip();

		// Any inputs?
		while (input->CheckForEvent()) {
			auto k = input->GetEvent();
			switch (k.type) {
				case EventType::KeyDown: {
					switch (k.keyCode) {
						case 113: // F1
							Compile(true, true, true);
							WriteContent();
							Header();
							break;
						case 114: // F2
							Compile(true, true, false);
							WriteContent();
							Header();
							break;
						case 115: // F3
							break;
						case 20: // F4
							break;
						case 118: // F8
							break;
						case 119: // F9
							break;
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

bool Editor::Compile(bool run, bool wait, bool optimise)
{
	// Collate editor lines into a string
	std::string code;
	for (auto l: lines) {
		code += l;
		code += '\n';
	}

	row = 0;
	try {
		// Tokens
		token.Parse("[Interactive]", code);

		parser.Parse(optimise, token.Tokens());
		if (!optimise) {
			std::list<std::string> tokens;
			token.PrintTokensPtr(parser.Tokens(), 0, &tokens);
#ifdef CLION
			std::ofstream tokens_out("../../tokens.txt");
			for (auto& s:tokens) {
				tokens_out << s << std::endl;
			}
			tokens_out.close();
#else
			PRINTC("\nTokens\n------\n");
			for (auto &s: tokens)
				PRINTC("%s\n", s.c_str());
#endif
		}

		// Compile
		IRCompiler ir_compiler(optimise);
		ir_compiler.Compile(parser.Tokens());
		if (!optimise) {
			std::list<std::string> ir;
			ir_compiler.IRPrinter(&ir);
#ifdef CLION
			std::ofstream ir_out("../../ir.txt");
			for (auto& s:ir) {
				ir_out << s << std::endl;
			}
			ir_out.close();
#else
			PRINTC("\nIR\n--\n");
			for (auto &s: ir)
				PRINTC("%s\n", s.c_str());
#endif
		}

		// Native
		if (run) {
			NativeCompiler native_compiler(optimise);
			native_compiler.IRToNative(ir_compiler.GetGlobalIRInstructions(), ir_compiler.GetIRInstructions());
			if (!optimise) {
				std::list<std::string> disassm;
				native_compiler.Disassemble(&disassm);
#ifdef CLION
				std::ofstream native_out("../../native.txt");
				for (auto& s:disassm) {
					native_out << s << std::endl;
				}
				native_out.close();
#else
				PRINTC("\nCode\n----\n");
				for (auto &s: disassm)
					PRINTC("%s\n", s.c_str());
#endif
			}

			Console_SetFG(CONSOLE_WHITE);
			Console_SetBG(CONSOLE_BLACK);
			Console_SetPosition(0, 0);
			Console_Clear();
			auto t1 = GetClock();
			GetCurrentTask()->RunCode();
			double t2 = (double)(GetClock()-t1)/100.0;
			std::ostringstream out;
			out.precision(2);
			out << std::fixed << t2;

			// End message and wait for key
			std::string finish_message = "Run time was "+out.str()+" seconds";
			Console_SetPosition((Console_Width()-finish_message.length())/2, Console_Height()-1);
			Console_SetFG(CONSOLE_BLACK);
			Console_SetBG(CONSOLE_YELLOW);
			Console_WriteString(finish_message.c_str());
			Console_Flip();
			if (wait)
				input->Get();
		}
	}
	catch (DARICException& ex) {
		Console_SetBG(CONSOLE_RED);
		Console_SetLineBG(0, Console_Width(), Console_Height()-1);
		Console_SetPosition(0, Console_Height()-1);
		Console_SetFG(CONSOLE_YELLOW);
		switch (ex.type) {
			case ExceptionType::COMPILER:
				Console_WriteString("[Compiler] ");
				break;
			case ExceptionType::TOKENISER:
				Console_WriteString("[Tokeniser] ");
				break;
			case ExceptionType::PARSER:
				Console_WriteString("[Parser] ");
				break;
			case ExceptionType::RUNTIME:
				Console_WriteString("[Runtime] ");
				break;
		}
		Console_SetFG(CONSOLE_WHITE);
		Console_WriteString(ex.error.c_str());
		Console_WriteString(" at ");
		Console_WriteString("line %d, column %d", ex.line_number, ex.char_position);
		row = ex.line_number-10;
		if (row<0)
			row = 0;
		error_lines.insert(ex.line_number);
		errors.push_back(std::move(ex));
		return false;
	}

	return true;
}

void Editor::Header()
{
	auto y = 0;
	// Row and col
	Console_SetFG(CONSOLE_WHITE);
	Console_SetBG(CONSOLE_DARKERGREY);
	Console_SetLineBG(0, Console_Width(), y);
	char b[64];
	sprintf(b, "Ln %d, Col %d, %d line(s)", row, col, (int)lines.size());
	auto pos = Console_Width()-strlen(b)-1;
	Console_SetPosition(pos, y);
	Console_WriteString("%s", &b);

#ifndef CLION
	Console_SetFG(CONSOLE_WHITE);
	Console_SetPosition(0, y);
	Console_WriteString("OS/D DARIC Code Editor");
	Console_SetFG(CONSOLE_DARKGREY);
	Console_WriteString(" | ");
	Console_SetFG(CONSOLE_WHITE);
	Console_WriteString("RAM: %lldMB/%lldMB", memory->GetHeapFreeSpace(HEAP_ANY) / 1024 / 1024, memory->GetMemSize() / 1024 / 1024);
#else
	Console_SetFG(CONSOLE_WHITE);
	Console_SetPosition(0, y);
	Console_WriteString("OS/D DARIC Code Editor");
#endif

}

void Editor::WriteContent()
{
	// Blank out content area
	for (auto i = 2; i<Console_Height()-(int)errors.size(); i++) {
		Console_SetBG(CONSOLE_DARKERGREY);
		Console_SetLineBG(0, 9, i);
		Console_SetBG(CONSOLE_BLACK);
		Console_SetLineBG(9, Console_Width(), i);
	}

	int trow = row;
	std::list<std::string>::iterator it = lines.begin();
	std::list<std::string>::iterator end = lines.end();

	// Skip lines before
	for (int i = 0; i<trow; i++) {
		if (it==end)
			return;
		++it;
	}

	// Columns
	Console_SetBG(CONSOLE_DARKERGREY);
	Console_SetFG(CONSOLE_WHITE);
	Console_SetLineBG(0, Console_Width(), 2);
	Console_SetLineBG(0, Console_Width(), 1);
	for (auto i = 9; i<Console_Width(); i++) {
		Console_SetPosition(i, 2);
		Console_WriteString(".");
	}
	for (auto i = 18; i<Console_Width()-3; i += 10) {
		Console_SetPosition(i, 2);
		Console_WriteString("|", i-8);
		Console_SetFG(CONSOLE_YELLOW);
		Console_SetPosition(i+1, 2);
		Console_WriteString("%d", i-8);
		Console_SetFG(CONSOLE_WHITE);
	}

	// Let us output as many lines we can
	Console_SetFG(CONSOLE_WHITE);
	Console_SetBG(CONSOLE_BLUE);
	for (auto i = 3; i<Console_Height()-(int)errors.size(); i++) {
		if (it==end)
			break;

		// Draw row
		Console_SetBG(CONSOLE_DARKERGREY);
		Console_SetPosition(0, i);
		Console_SetFG(CONSOLE_WHITE);
		Console_WriteString("[");
		Console_SetFG(CONSOLE_YELLOW);
		Console_WriteString("%6d", i+row-2);
		Console_SetFG(CONSOLE_WHITE);
		if (error_lines.count(i+5)>0) {
			Console_WriteString("*]");
		}
		else {
			Console_WriteString(" ]");
		}
		Console_SetBG(CONSOLE_BLACK);
		Console_SetFG(CONSOLE_WHITE);
		Console_WriteString(" ");
		Console_WriteStringNoFormatting((*it).c_str());

		++it;
	}

	// Show error position
	for (auto error: errors) {
		Console_SetBG(CONSOLE_RED);
		Console_SetFG(CONSOLE_WHITE);

		auto x = error.char_position+8;
		auto y = error.line_number-trow-1;
		char c = Console_GetCharacter(x, y+3);
		Console_SetLineBG(x, x+1, y+3);
		Console_SetPosition(x, y+3);
		Console_WriteString("%c", c);
	}
}