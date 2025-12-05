#pragma once

#include <filesystem>
#include <optional>
#include <ostream>
#include <string>

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
	solver::linear::LinearSolverType LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;

	std::string ToString() const
	{
		std::ostringstream oss;

		oss << "Application Configuration:\n";
		oss << "  Show Help: " << (showHelp ? "Yes" : "No") << "\n";
		oss << "  Log Level: " << spdlog::level::to_string_view(logLevel).data() << "\n";
		oss << "  Config File: " << (configFilePath.empty() ? "<not set>" : configFilePath.string()) << "\n";
		oss << "  Number of Threads: " << (numberOfThreads.has_value() ? std::to_string(numberOfThreads.value()) : "auto") << "\n";
		oss << "  Linear Solver: " << solver::linear::LinearSolverTypeToString(LinearSolverType);

		return oss.str();
	}
};

} // namespace fem::core
