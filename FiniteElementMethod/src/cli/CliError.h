#pragma once

#include <filesystem>
#include <string>

namespace fem::cli
{

enum class CliErrorCode
{
	ParsingError,
	Unknown
};

struct CliError
{
	CliErrorCode code;
	std::string message;

	friend std::ostream& operator<<(std::ostream& os, const CliError& err)
	{
		os << err.message;
		return os;
	}
};

}
