#include "NativeCompiler.h"
#ifndef CLION
#include <circle/logger.h>
#endif

static bool tabbed = true;
const int tab_size = 20;

void call_PRINT_NL()
{
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintNewLine;
#ifdef CLION
	printf("\n");
#endif
	task->CallGUIDirectEx(&mess);
}

void call_PRINT_Tabbed()
{
	tabbed = true;
}

void call_PRINT_TabbedOff()
{
	tabbed = false;
}

void call_PRINT_integer(int64_t v)
{
	char d[64];
	if (tabbed) {
		sprintf(d, "%" PRId64 "", v);
		auto pos = 20-strlen(d);
		memset(d, ' ', 64);
		sprintf(&d[pos], "%" PRId64 "", v);
	}
	else {
		sprintf(d, "%" PRId64 "", v);
	}

	// Send
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	mess.data = &d;
	task->CallGUIDirectEx(&mess);
	tabbed = false;
}

void call_PRINT_real(double v)
{
	char d[64];
	if (tabbed) {
		sprintf(d, "%f", v);
		auto pos = 20-strlen(d);
		memset(d, ' ', 64);
		sprintf(&d[pos], "%f", v);
	}
	else {
		sprintf(d, "%f", v);
	}

	// Send
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	mess.data = &d;
#ifdef CLION
	printf("%s", d);
#endif
	task->CallGUIDirectEx(&mess);
	tabbed = false;
}

void call_PRINT_string(int64_t idx)
{
	auto v = OS_Strings_Get(idx);

	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	mess.data = (void*)v.c_str();
	task->CallGUIDirectEx(&mess);
#ifdef CLION
	printf("%s", v.c_str());
#endif
	tabbed = false;
	task->ClearTemporaryStrings();
}

void call_PRINT_SPC(int64_t v)
{
	char s[v+1];
	memset(s, ' ', v);
	s[v+1] = 0;

	// Send
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	mess.data = &s;
	task->CallGUIDirectEx(&mess);
}

void call_PRINT_TAB(int64_t v)
{
	// Send
	auto task = GetCurrentTask();
	DirectMessage mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintTab;
	mess.data = &v;
	task->CallGUIDirectEx(&mess);
}
