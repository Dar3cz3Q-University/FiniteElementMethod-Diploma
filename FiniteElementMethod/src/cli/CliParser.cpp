#include "CliParser.h"

#include "core/core.h"
#include "solver/solver.h"

#include <exception>
#include <format>
#include <iostream>

#include <cxxopts.hpp>
#include <fmt/ranges.h>

namespace fem::cli
{

std::expected<core::ApplicationOptions, CliError> CliParser::Parse(int argc, const char* const* argv)
{
	cxxopts::Options options("Finite Element Method", "Heat transfer solver using FEM");

	options.add_options()
		("h,help", "Show help message")
		("i,input", "Input mesh file path",
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
	catch (const cxxopts::exceptions::parsing& err)
	{
		return std::unexpected(
			CliError{
				CliErrorCode::ParsingError,
				err.what()
			}
		);
	}

	fem::core::ApplicationOptions config{};

	//
	// Help
	//

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		config.ShowHelp = true;
		return config;
	}

	//
	// Input file
	//

	if (!result.count("input"))
	{
		return std::unexpected(
			CliError{
				CliErrorCode::RequiredArgumentMissing,
				"Input mesh file is required (use -i or --input)"
			}
		);
	}

	config.MeshInputPath = result["input"].as<std::string>();

	if (!std::filesystem::exists(config.MeshInputPath))
	{
		return std::unexpected(
			CliError{
				CliErrorCode::FileError,
				"Mesh file not found: " + config.MeshInputPath.string()
			}
		);
	}

	//
	// Threads
	//

	if (result.count("threads"))
	{
		config.NumberOfThreads = result["threads"].as<std::size_t>();
	}

	//
	// Solver type
	//

	std::string solverStr = result["solver"].as<std::string>();
	auto solverType = fem::solver::linear::ParseSolverType(solverStr);
	if (!solverType)
	{
		return std::unexpected(
			CliError{
				CliErrorCode::InvalidValue,
				std::format("Invalid solver type '{}'", solverStr)
			}
		);
	}
	config.LinearSolverType = *solverType;

	return config;
}

std::string CliParser::GenerateSolverHelpText()
{
	using namespace solver::linear;

	std::vector<std::string_view> names;
	names.reserve(LINEAR_SOLVERS.size());

	for (const auto& solver : LINEAR_SOLVERS)
	{
		names.push_back(solver.name);
	}

	return std::format("{}", fmt::join(names, ", "));
}

} // namespace fem::cli
