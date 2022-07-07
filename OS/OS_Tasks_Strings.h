#pragma once
#include <string>
#include <map>
#include <set>

class OSDTaskStrings
{
  public:
    void FreeString(int64_t index);
    int64_t AddString(std::string s);
    int64_t AddStringPermanent(std::string s);
    std::string &GetString(int64_t idx);
    void ClearTemporaryStrings();
    void MakeStringPermanent(int64_t idx);
    void FreeStringPermanent(int64_t idx);
    void SetConstantString(int64_t idx);
    size_t GetStringCount()
    {
        return permanent_strings.size();
    }
    size_t GetStringCountTemporary()
    {
        return strings.size();
    }

  private:
    int64_t string_index = 1;
    std::map<int64_t, std::string> strings;
    std::map<int64_t, std::string> permanent_strings;
    std::set<int64_t> constant_strings;
};