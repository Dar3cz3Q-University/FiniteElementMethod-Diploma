#pragma once

#include <format>
#include <string>

namespace fem::config::loader
{

/// <summary>
/// Configuration loading error types.
/// </summary>
enum class ConfigLoaderErrorCode : int
{
	/// <summary>
	/// File-related error (not found, cannot read, etc.)
	/// </summary>
	FileError = 0,

	/// <summary>
	/// JSON parsing error (invalid syntax)
	/// </summary>
	ParserError,

	/// <summary>
	/// Required field missing in JSON
	/// </summary>
	MissingRequiredField,

	/// <summary>
	/// Field value is invalid or out of range
	/// </summary>
	InvalidValue,

	/// <summary>
	/// Unspecified or unexpected error
	/// </summary>
	Unknown,
};

/// <summary>
/// Represents an error reported by the FEM configuration loader
/// </summary>
struct ConfigLoaderError
{
	/// <summary>
	/// Error type code
	/// </summary>
	ConfigLoaderErrorCode code;

	/// <summary>
	/// Detailed error description
	/// </summary>
	std::string message;

	/// <summary>
	/// Convert error to human-readable string
	/// </summary>
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
