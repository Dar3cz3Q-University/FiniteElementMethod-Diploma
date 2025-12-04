#pragma once

#include <filesystem>
#include <optional>

#include "solver/solver.h"
#include "logger/logger.h"

namespace fem::core
{

/// <summary>
/// Represents configuration options for the FEM application,
/// typically populated from command-line arguments.
/// </summary>
struct ApplicationOptions
{
	/// <summary>
	/// Indicates whether the application should display the help page and exit.
	/// </summary>
	bool ShowHelp = false;

	/// <summary>
	/// Specifies the current spdlog logging level.
	/// </summary>
	spdlog::level::level_enum LogLevel = spdlog::level::info;

	/// <summary>
	/// Path to the input file containing mesh.
	/// </summary>
	std::filesystem::path MeshInputPath;

	/// <summary>
	/// Number of threads to use for processing.
	/// </summary>
	std::optional<std::size_t> NumberOfThreads;

	/// <summary>
	/// Type of problem being solved.
	/// </summary>
	solver::ProblemType ProblemType = solver::ProblemType::Steady; // TODO: Move to JSON config later

	/// <summary>
	/// Type of direct solver used for the sparse system.
	/// </summary>
	solver::SolverType SolverType = solver::SolverType::SimplicialLDLT;
};

} // namespace fem::core
