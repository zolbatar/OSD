#pragma once
#include <string>
#include <exception>
#include <stdexcept>

enum class ExceptionType {
	TOKENISER,
	PARSER,
	COMPILER,
	RUNTIME,
};

class DARICException : public std::runtime_error {
public:
	DARICException(ExceptionType type, std::string filename, uint32_t line_number, short char_position, std::string error)
			:std::runtime_error(error), type(type), error(error), filename(filename), line_number(line_number), char_position(char_position)
	{
	};

	const char* what() const throw()
	{
		return error.c_str();
	}

	ExceptionType type;
	std::string error;
	std::string filename;
	uint32_t line_number;
	short char_position;
};