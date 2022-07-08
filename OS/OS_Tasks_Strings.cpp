#include "OS_Tasks_Strings.h"

void OSDTaskStrings::FreeString(int64_t index)
{
    strings.erase(index);
}

int64_t OSDTaskStrings::AddString(std::string s)
{
    auto i = string_index++;
    //    CLogger::Get()->Write("OSDTask", LogDebug, "Create string: '%s'/%d", s.c_str(), i);
    strings.insert(std::make_pair(i, std::move(s)));
    return i;
}

int64_t OSDTaskStrings::AddStringPermanent(std::string s)
{
    // Do we have an existing one the same?
    for (auto &str : permanent_strings)
    {
        if (str.second == s)
        {
            return str.first;
        }
    }
    auto i = string_index++;
    //    CLogger::Get()->Write("OSDTask", LogDebug, "Create permanent string: '%s'/%d", s.c_str(), i);
    constant_strings.insert(i);
    permanent_strings.insert(std::make_pair(i, std::move(s)));
    return i;
}

void OSDTaskStrings::MakeStringPermanent(int64_t idx)
{
    auto f = strings.extract(idx);
    permanent_strings.insert(std::move(f));
}

void OSDTaskStrings::SetConstantString(int64_t idx)
{
    constant_strings.insert(idx);
}

void OSDTaskStrings::FreeStringPermanent(int64_t idx)
{
    if (constant_strings.count(idx) > 0)
        return;
    auto f = permanent_strings.extract(idx);
    strings.insert(std::move(f));
}

void OSDTaskStrings::ClearTemporaryStrings()
{
    strings.clear();
}

std::string &OSDTaskStrings::GetString(int64_t idx)
{
    auto f = strings.find(idx);
    if (f == strings.end())
    {
        auto f = permanent_strings.find(idx);
        if (f == permanent_strings.end())
        {
            CLogger::Get()->Write("OSDTask", LogDebug, "Temporary strings:");
            for (auto &s : strings)
            {
                CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
            }
            CLogger::Get()->Write("OSDTask", LogDebug, "Permanent strings:");
            for (auto &s : permanent_strings)
            {
                CLogger::Get()->Write("OSDTask", LogDebug, "%d = '%s'", s.first, s.second.c_str());
            }
            CLogger::Get()->Write("OSDTask", LogPanic, "Invalid string: %d", idx);
        }
        return f->second;
    }
    return f->second;
}
