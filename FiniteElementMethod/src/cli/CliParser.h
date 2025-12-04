#pragma once

#include "CliError.h"

#include "core/core.h"

#include <expected>
#include <string>

#include <cxxopts.hpp>

namespace fem::cli
{

class CliParser
{
public:
	CliParser() = default;

	/// <summary>
	/// Parse command-line arguments into application configuration
	/// </summary>
	/// <param name="argc">Argument count</param>
	/// <param name="argv">Argument values</param>
	/// <returns>Parsed configuration or error</returns>
	static std::expected<core::ApplicationOptions, CliError> Parse(int argc, const char* const* argv);

private:
	static std::string GenerateSolverHelpText();

	static std::expected<void, CliError> ExtractConfigFilePath(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
	static std::expected<void, CliError> ExtractThreadsNumber(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
	static std::expected<void, CliError> ExtractSolverType(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
};

} // namespace fem::cli
