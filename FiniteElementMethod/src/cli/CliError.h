#pragma once

#include <filesystem>
#include <format>
#include <string>

namespace fem::cli
{

/// <summary>
/// Error codes used by the FEM command-line interface (CLI).
/// </summary>
enum class CliErrorCode
{
	/// <summary>
	/// Error occurred while parsing command-line arguments or input data.
	/// </summary>
	ParsingError = 0,

	/// <summary>
	/// A required argument is missing.
	/// </summary>
	RequiredArgumentMissing,

	/// <summary>
	/// Invalid argument value provided.
	/// </summary>
	InvalidValue,

	/// <summary>
	/// File-related error (not found, cannot read, etc.)
	/// </summary>
	FileError,

	/// <summary>
	/// Unspecified or unexpected error type.
	/// </summary>
	Unknown
};

/// <summary>
/// Represents an error reported by the FEM command-line interface (CLI).
/// </summary>
struct CliError
{
	/// <summary>
	/// Error code describing the type of the CLI error.
	/// </summary>
	CliErrorCode code;

	/// <summary>
	/// Human-readable description of the error. May be empty.
	/// </summary>
	std::string message;

	/// <summary>
	/// Converts a <see cref="CliError"/> instance to a human-readable string representation.
	/// </summary>
	/// <param name="err">CLI error to be converted.</param>
	/// <returns>String containing the error code and optional message.</returns>
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

/// <summary>
/// std::formatter specialization for <see cref="fem::cli::CliError"/> enabling usage with std::format.
/// </summary>
template <>
struct std::formatter<fem::cli::CliError> : std::formatter<std::string_view>
{
	auto format(const fem::cli::CliError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(err.ToString(), ctx);
	}
};
