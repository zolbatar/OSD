#include "NativeCompiler.h"
#include <sstream>

int64_t call_STRING_equal(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r==0;
}

int64_t call_STRING_notequal(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r!=0;
}

int64_t call_STRING_less(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r<0;
}

int64_t call_STRING_lessequal(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r<=0;
}

int64_t call_STRING_greater(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r>0;
}

int64_t call_STRING_greaterequal(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.compare(s2s);
	return r>=0;
}

int64_t call_STRING_add(int64_t s1, int64_t s2)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto s3s = s1s+s2s;
	auto r = OS_Strings_Create(s3s);
	return r;
}

int64_t call_STRING_asc(int64_t s)
{
	auto ss = OS_Strings_Get(s);
	int64_t r;
	if (ss.length()==0)
		r = -1;
	else
		r = ss[0];
	return r;
}

int64_t call_STRING_chrs(int64_t s)
{
	std::string ss;
	ss += s;
	auto r = OS_Strings_Create(ss);
	return r;
}

int64_t call_STRING_instr(int64_t s3, int64_t s2, int64_t s1)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = OS_Strings_Get(s2);
	auto r = s1s.find(s2s, s3);
	if (r==std::string::npos)
		return -1;
	return r+1;
}

int64_t call_STRING_lefts(int64_t s2, int64_t s1)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = s1s.substr(0, s2);
	auto r = OS_Strings_Create(s2s);
	return r;
}

int64_t call_STRING_mids(int64_t s3, int64_t s2, int64_t s1)
{
	auto s1s = OS_Strings_Get(s1);
	try {
		auto s2s = s1s.substr(s2-1, s3);
		auto r = OS_Strings_Create(s2s);
		return r;
	}
	catch (const std::out_of_range& e) {
		std::string ss;
		auto r = OS_Strings_Create(ss);
		return r;
	}
}

int64_t call_STRING_rights(int64_t s2, int64_t s1)
{
	auto s1s = OS_Strings_Get(s1);
	auto s2s = s1s.substr(s1s.length()-s2, s2);
	auto r = OS_Strings_Create(s2s);
	return r;
}

int64_t call_STRING_len(int64_t s)
{
	auto ss = OS_Strings_Get(s);
	return ss.length();
}

int64_t call_STRING_strings(int64_t s2, int64_t s1)
{
	auto s1s = OS_Strings_Get(s1);
	std::string s2s = "";
	for (int i = 0; i<s2; i++) {
		s2s += s1s;
	}
	auto r = OS_Strings_Create(s2s);
	return r;
}

double call_STRING_stringToFloat(int64_t s)
{
	auto ss = OS_Strings_Get(s);
	try {
		double v = std::stod(ss);
		return v;
	}
	catch (const std::invalid_argument&) {
		return 0.0;
	}
}

int64_t call_STRING_stringToInt(int64_t s)
{
	auto ss = OS_Strings_Get(s);
	try {
		int64_t v = std::stoi(ss);
		return v;
	}
	catch (const std::invalid_argument&) {
		return 0.0;
	}
}

int64_t call_STRING_realtostring(double v)
{
	char d[64];
	sprintf(d, "%f", v);
	std::string ss(d);
	auto r = OS_Strings_Create(ss);
	return r;
}
