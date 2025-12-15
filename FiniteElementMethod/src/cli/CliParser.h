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

	static std::expected<core::ApplicationOptions, CliError> Parse(int argc, const char* const* argv);

private:
	static std::string GenerateSolverHelpText();

	static std::expected<void, CliError> ExtractConfigFilePath(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
	static std::expected<void, CliError> ExtractMetricsFilePath(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
	static std::expected<void, CliError> ExtractThreadsNumber(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
	static std::expected<void, CliError> ExtractSolverType(const cxxopts::ParseResult& result, core::ApplicationOptions* config);
};

} // namespace fem::cli
