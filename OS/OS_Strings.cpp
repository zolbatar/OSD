#include "OS.h"

int64_t OS_Strings_Create(std::string& s)
{
	return GetCurrentTask()->AddString(s);
}

std::string& OS_Strings_Get(int64_t idx)
{
	return GetCurrentTask()->GetString(idx);
}