#include "NativeCompiler.h"
#ifndef CLION
#include <circle/logger.h>
#endif

static bool tabbed = true;
const int tab_size = 20;

void call_PRINT_NL()
{
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintNewLine;
#ifdef CLION
	printf("\n");
#endif
	task->SendGUIMessage(std::move(mess));
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
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	strcpy((char*)&mess.data, d);
#ifdef CLION
	printf("%s", d);
#endif
	task->SendGUIMessage(std::move(mess));

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
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	strcpy((char*)&mess.data, d);
#ifdef CLION
	printf("%s", d);
#endif
	task->SendGUIMessage(std::move(mess));

	tabbed = false;
}

void call_PRINT_string(int64_t idx)
{
	auto v = OS_Strings_Get(idx);

	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	if (v.length()<=MESSAGE_BLOCK) {
		mess.type = Messages::Canvas_PrintString;
		strcpy((char*)&mess.data, v.c_str());
	}
	else {
		mess.type = Messages::Canvas_PrintStringLong;
		auto m = (Address*)&mess.data;
		m->address = (void*)v.c_str();
	}
	task->SendGUIMessage(std::move(mess));
#ifdef CLION
	printf("%s", v.c_str());
#endif
	tabbed = false;
}

void call_PRINT_SPC(int64_t v)
{
	char s[v+1];
	memset(s, ' ', v);
	s[v+1] = 0;

	// Send
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintString;
	strcpy((char*)&mess.data, s);
	task->SendGUIMessage(std::move(mess));
}

void call_PRINT_TAB(int64_t v)
{
	// Send
	auto task = GetCurrentTask();
	Message mess;
	mess.source = task;
	mess.type = Messages::Canvas_PrintTab;
	auto m = (Integer*)&mess.data;
	m->v = v;
	task->SendGUIMessage(std::move(mess));
}
