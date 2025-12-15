#pragma once

#include <format>
#include <string>

namespace fem::config::loader
{

enum class ConfigLoaderErrorCode : int
{
	FileError = 0,
	ParserError,
	MissingRequiredField,
	InvalidValue,
	Unknown,
};

struct ConfigLoaderError
{
	ConfigLoaderErrorCode code;
	std::string message;

	std::string ToString() const
	{
		using enum ConfigLoaderErrorCode;

		std::string_view errorType;
		switch (code)
		{
		case FileError:
			errorType = "File error";
			break;
		case ParserError:
			errorType = "JSON parsing error";
			break;
		case MissingRequiredField:
			errorType = "Missing required field";
			break;
		case InvalidValue:
			errorType = "Invalid value";
			break;
		case Unknown:
			errorType = "Unknown error";
			break;
		default:
			std::unreachable();
		}

		if (!message.empty())
		{
			return std::format("ConfigLoaderError: {}. Message: {}", errorType, message);
		}

		return std::format("ConfigLoaderError: {}.", errorType);
	}
};

}
