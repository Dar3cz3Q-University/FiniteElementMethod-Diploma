#include "CliParser.h"

#include "core/core.h"
#include "solver/solver.h"

#include <exception>
#include <iostream>

#include <cxxopts.hpp>

namespace fem::cli
{

std::expected<fem::core::ApplicationOptions, CliError> CliParser::Parse(int argc, const char* const* argv)
{
	cxxopts::Options options("Finite Element Method", "Heat transfer solver using FEM");

	std::string solverHelp = "Linear solver: ";
	for (size_t i = 0; i < fem::solver::LINEAR_SOLVERS.size(); ++i)
	{
		solverHelp += fem::solver::LINEAR_SOLVERS[i].name;
		if (i < fem::solver::LINEAR_SOLVERS.size() - 1)
			solverHelp += ", ";
	}

	std::string problemHelp = "Problem type: ";
	for (size_t i = 0; i < fem::solver::PROBLEM_TYPES.size(); ++i)
	{
		problemHelp += fem::solver::PROBLEM_TYPES[i].name;
		if (i < fem::solver::PROBLEM_TYPES.size() - 1)
			problemHelp += ", ";
	}

	options.add_options()
		("h,help", "Show help message")
		("i,input", "Input mesh file path",
			cxxopts::value<std::string>())
		("t,threads", "Number of threads (default: hardware concurrency)",
			cxxopts::value<std::size_t>())
		("p,problem", problemHelp,
			cxxopts::value<std::string>()->default_value("steady"))
		("s,solver", solverHelp,
			cxxopts::value<std::string>()->default_value("cholesky"));

	cxxopts::ParseResult result;
	try
	{
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::parsing& err)
	{
		return std::unexpected(CliError{ CliErrorCode::ParsingError, err.what() });
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
		return std::unexpected(CliError{
			CliErrorCode::RequiredArgumentMissing,
				"Input mesh file is required (use -i or --input)"
			});
	}

	config.MeshInputPath = result["input"].as<std::string>();

	if (!std::filesystem::exists(config.MeshInputPath))
	{
		return std::unexpected(CliError{
			CliErrorCode::FileError,
				"Mesh file not found: " + config.MeshInputPath.string()
			});
	}

	//
	// Threads
	//

	if (result.count("threads"))
	{
		config.NumberOfThreads = result["threads"].as<std::size_t>();
	}

	//
	// Problem type
	//

	std::string problemStr = result["problem"].as<std::string>();
	auto problemType = solver::ParseProblemType(problemStr);
	if (!problemType)
	{
		return std::unexpected(CliError{
			CliErrorCode::InvalidValue,
				std::format("Invalid problem type '{}'", problemStr)
			});
	}
	config.ProblemType = *problemType;

	//
	// Solver type
	//

	std::string solverStr = result["solver"].as<std::string>();
	auto solverType = fem::solver::ParseSolverType(solverStr);
	if (!solverType)
	{
		return std::unexpected(CliError{
			CliErrorCode::InvalidValue,
				std::format("Invalid solver type '{}'", solverStr)
			});
	}
	config.SolverType = *solverType;

	return config;
}

} // namespace fem::cli
