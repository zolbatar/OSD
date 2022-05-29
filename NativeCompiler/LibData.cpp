#include "NativeCompiler.h"

void call_DATA_integer(int64_t v)
{
	DataElement de;
	de.type = ValueType::Integer;
	de.iv = v;
	GetCurrentTask()->AddDataElement(std::move(de));
}

void call_DATA_float(double v)
{
	DataElement de;
	de.type = ValueType::Float;
	de.rv = v;
	GetCurrentTask()->AddDataElement(std::move(de));
}

void call_DATA_string(int64_t v)
{
	DataElement de;
	de.type = ValueType::String;
	de.sv = v;
	GetCurrentTask()->AddDataElement(std::move(de));
	GetCurrentTask()->SetConstantString(v); // Is this necessary?
}

int64_t call_READ_integer()
{
	auto de = GetCurrentTask()->GetDataElement();
	switch (de->type) {
		case ValueType::Integer:
			return de->iv;
		case ValueType::Float:
			return static_cast<int64_t>(de->rv);
		case ValueType::String:
			return 0;
	}
}

double call_READ_float()
{
	auto de = GetCurrentTask()->GetDataElement();
	switch (de->type) {
		case ValueType::Integer:
			return static_cast<double>(de->iv);
		case ValueType::Float:
			return de->rv;
		case ValueType::String:
			return 0;
	}
}

int64_t call_READ_string()
{
	auto de = GetCurrentTask()->GetDataElement();
	switch (de->type) {
		case ValueType::Integer: {
			std::string v = std::to_string(de->iv);
			return OS_Strings_Create(v);
		}
		case ValueType::Float: {
			std::string v = std::to_string(de->iv);
			return OS_Strings_Create(v);
		}
		case ValueType::String:
			return de->sv;
	}
}

void call_DATA_label(int64_t v)
{
	auto s = OS_Strings_Get(v);
	GetCurrentTask()->AddDataLabel(s);
}

void call_DATA_restore(int64_t v)
{
	auto s = OS_Strings_Get(v);
	GetCurrentTask()->RestoreDataLabel(s);
}
