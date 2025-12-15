#include "CliParser.h"

#include "core/core.h"
#include "solver/solver.h"

#include <exception>
#include <format>
#include <iostream>
#include <print>

#include <fmt/ranges.h>

namespace fem::cli
{

std::expected<core::ApplicationOptions, CliError> CliParser::Parse(int argc, const char* const* argv)
{
	cxxopts::Options options("Finite Element Method", "Heat transfer solver using FEM");

	options.add_options()
		("h,help", "Show help message")
		("i,input", "Input config file path",
			cxxopts::value<std::string>())
		("m,metrics", "Metrics file path",
			cxxopts::value<std::string>())
		("t,threads", "Number of threads (default: hardware concurrency)",
			cxxopts::value<std::size_t>())
		("s,solver", GenerateSolverHelpText(),
			cxxopts::value<std::string>()->default_value("cholesky"));

	cxxopts::ParseResult result;
	try
	{
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::exception& e)
	{
		return std::unexpected(
			CliError{
				CliErrorCode::ParsingError,
				e.what()
			}
		);
	}

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		return core::ApplicationOptions{ .showHelp = true };
	}

	core::ApplicationOptions config{};

	if (auto res = ExtractConfigFilePath(result, &config); !res)
		return std::unexpected(res.error());

	if (auto res = ExtractMetricsFilePath(result, &config); !res)
		return std::unexpected(res.error());

	if (auto res = ExtractThreadsNumber(result, &config); !res)
		return std::unexpected(res.error());

	if (auto res = ExtractSolverType(result, &config); !res)
		return std::unexpected(res.error());

	return config;
}

std::string CliParser::GenerateSolverHelpText()
{
	using namespace solver::linear;

	std::vector<std::string_view> names;
	names.reserve(LINEAR_SOLVERS.size());

	for (const auto& solver : LINEAR_SOLVERS)
		names.push_back(solver.name);

	return fmt::format("Linear solver: {}", fmt::join(names, ", "));
}

std::expected<void, CliError> CliParser::ExtractConfigFilePath(const cxxopts::ParseResult& result, core::ApplicationOptions* config)
{
	if (!result.count("input"))
		return std::unexpected(
			CliError{
				CliErrorCode::RequiredArgumentMissing,
				"Input config file is required (use -i or --input)"
			}
		);

	config->configFilePath = result["input"].as<std::string>();

	if (!std::filesystem::exists(config->configFilePath))
		return std::unexpected(
			CliError{
				CliErrorCode::FileError,
				"Config file not found: " + config->configFilePath.string()
			}
		);

	return {};
}

std::expected<void, CliError> CliParser::ExtractMetricsFilePath(const cxxopts::ParseResult& result, core::ApplicationOptions* config)
{
	if (!result.count("metrics"))
		config->metricsFilePath = result["metrics"].as<std::string>();

	return {};
}

std::expected<void, CliError> CliParser::ExtractThreadsNumber(const cxxopts::ParseResult& result, core::ApplicationOptions* config)
{
	if (result.count("threads"))
		config->numberOfThreads = result["threads"].as<std::size_t>();

	return {};
}

std::expected<void, CliError> CliParser::ExtractSolverType(const cxxopts::ParseResult& result, core::ApplicationOptions* config)
{
	std::string solverStr = result["solver"].as<std::string>();
	auto solverType = solver::linear::ParseSolverType(solverStr);

	if (!solverType)
		return std::unexpected(
			CliError{
				CliErrorCode::InvalidValue,
				std::format("Invalid solver type '{}'", solverStr)
			}
		);

	config->LinearSolverType = *solverType;

	return {};
}

} // namespace fem::cli
