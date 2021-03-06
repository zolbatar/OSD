#include "OS.h"

void OS_Strings_Free(int64_t index)
{
    GetCurrentTask()->Strings.FreeString(index);
}

int64_t OS_Strings_Create(std::string &s)
{
    return GetCurrentTask()->Strings.AddString(s);
}

int64_t OS_Strings_CreatePermanent(std::string &s)
{
    return GetCurrentTask()->Strings.AddStringPermanent(s);
}

void OS_Strings_MakePermanent(int64_t s)
{
    GetCurrentTask()->Strings.MakeStringPermanent(s);
}

void OS_Strings_FreePermanent(int64_t s)
{
    // Uninitialised
    if (s == 0)
        return;
    GetCurrentTask()->Strings.FreeStringPermanent(s);
}

std::string &OS_Strings_Get(int64_t idx)
{
    return GetCurrentTask()->Strings.GetString(idx);
}
