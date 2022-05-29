#include "NativeCompiler.h"

#include <chrono>

extern std::chrono::system_clock::time_point t1;
extern uint64_t GetClock();

int64_t call_TIME()
{
	return GetClock();
}

int64_t call_TIMES()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%a,%e %b %Y.%T", timeinfo);
	std::string v(buffer);
	auto r = OS_Strings_Create(v);
	return r;
}
