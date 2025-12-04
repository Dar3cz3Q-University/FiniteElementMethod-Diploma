#pragma once

#include "CliError.h"

#include "core/core.h"

#include <expected>

namespace fem::cli
{

/// <summary>
/// Parses command-line arguments for the FEM application and produces application options.
/// </summary>
class CliParser
{
public:
	CliParser() = default;

	/// <summary>
	/// Parses command-line arguments and converts them into <see cref="fem::core::ApplicationOptions"/>.
	/// </summary>
	/// <param name="argc">Number of command-line arguments.</param>
	/// <param name="argv">Array of C-style strings containing the command-line arguments.</param>
	/// <returns>
	/// An <c>std::expected</c> containing <see cref="fem::core::ApplicationOptions"/> on success,
	/// or a <see cref="CliError"/> on failure.
	/// </returns>
	static std::expected<fem::core::ApplicationOptions, CliError> Parse(int argc, const char* const* argv);
};

} // namespace fem::cli
