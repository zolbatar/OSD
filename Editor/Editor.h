#pragma once
#include <list>
#include <string>
#include "../OS/OS.h"
#include "../Parser/Parser.h"
#include "../Tokeniser/Tokeniser.h"
#include "../IRCompiler/IRCompiler.h"
#include "../NativeCompiler/NativeCompiler.h"
#include "../Exception/DARICException.h"

class Editor
#ifndef CLION
	: public CTask
#endif
{
public:
	Editor();
	void Enter();
	void Run();

private:
	void Header();
	void WriteContent();
	bool Compile(bool run, bool wait, bool optimise);

	Tokeniser token;
	Parser parser;
	int col = 0;
	int row = 0;
	std::list<std::string> lines;
};
