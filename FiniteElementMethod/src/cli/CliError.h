#pragma once

#include <filesystem>
#include <format>
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
};

inline std::string ErrorToString(const CliError& err)
{
	std::string msg = "CliErrorCode: ";

	switch (err.code)
	{
	case CliErrorCode::ParsingError: msg += "Parsing error.\n"; break;
	case CliErrorCode::Unknown: msg += "Unknown error.\n"; break;
	}

	if (!err.message.empty())
	{
		msg += "(";
		msg += err.message;
		msg += ")";
	}

	return msg;
}

}

template <>
struct std::formatter<fem::cli::CliError> : std::formatter<std::string_view>
{
	auto format(const fem::cli::CliError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(fem::cli::ErrorToString(err), ctx);
	}
};
