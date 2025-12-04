#pragma once

#include <filesystem>
#include <optional>

#include "solver/solver.h"
#include "logger/logger.h"

namespace fem::core
{

/// <summary>
/// Represents configuration options for the FEM application
/// </summary>
struct ApplicationOptions
{
	/// <summary>
	/// Indicates whether the application should display the help page and exit
	/// </summary>
	bool showHelp = false;

	/// <summary>
	/// Specifies the current spdlog logging level
	/// </summary>
	spdlog::level::level_enum logLevel = spdlog::level::info;

	/// <summary>
	/// Path to the domain config file
	/// </summary>
	std::filesystem::path configFilePath;

	/// <summary>
	/// Number of threads to use for processing
	/// </summary>
	std::optional<std::size_t> numberOfThreads;

	/// <summary>
	/// Type of direct solver used for the sparse system
	/// </summary>
	solver::linear::linearSolverType linearSolverType = solver::linear::linearSolverType::SimplicialLDLT;
};

} // namespace fem::core
