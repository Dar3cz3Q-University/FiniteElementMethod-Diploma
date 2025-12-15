#pragma once

#include <filesystem>
#include <format>
#include <string>

namespace fem::cli
{

enum class CliErrorCode
{
	ParsingError = 0,
	RequiredArgumentMissing,
	InvalidValue,
	FileError,
	Unknown
};

struct CliError
{
	CliErrorCode code;
	std::string message;

	std::string ToString() const
	{
		using enum CliErrorCode;

		std::string_view errorType;
		switch (code)
		{
		case ParsingError:
			errorType = "Parsing error";
			break;
		case RequiredArgumentMissing:
			errorType = "Required argument missing";
			break;
		case InvalidValue:
			errorType = "Invalid value";
			break;
		case FileError:
			errorType = "File error";
			break;
		case Unknown:
			errorType = "Unknown error";
			break;
		default:
			std::unreachable();
		}

		if (!message.empty())
		{
			return std::format("CliErrorCode: {}. Message: {}", errorType, message);
		}

		return std::format("CliErrorCode: {}.", errorType);
	}
};

} // namespace fem::cli

template <>
struct std::formatter<fem::cli::CliError> : std::formatter<std::string_view>
{
	auto format(const fem::cli::CliError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(err.ToString(), ctx);
	}
};
