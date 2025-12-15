#pragma once

#include <filesystem>
#include <optional>
#include <ostream>
#include <string>

#include "solver/solver.h"
#include "logger/logger.h"

namespace fem::core
{

struct ApplicationOptions
{
	bool showHelp = false;
	spdlog::level::level_enum logLevel = spdlog::level::info;
	std::filesystem::path configFilePath;
	std::optional<std::filesystem::path> metricsFilePath;
	std::optional<std::size_t> numberOfThreads;
	solver::linear::LinearSolverType LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;

	std::string ToString() const
	{
		std::ostringstream oss;

		oss << "Application Configuration:\n";
		oss << "  Show Help: " << (showHelp ? "Yes" : "No") << "\n";
		oss << "  Log Level: " << spdlog::level::to_string_view(logLevel).data() << "\n";
		oss << "  Config File: " << (configFilePath.empty() ? "<not set>" : configFilePath.string()) << "\n";
		oss << "  Metrics File: " << (metricsFilePath.has_value() ? metricsFilePath->string() : "<not set>") << "\n";
		oss << "  Number of Threads: " << (numberOfThreads.has_value() ? std::to_string(numberOfThreads.value()) : "auto") << "\n";
		oss << "  Linear Solver: " << solver::linear::LinearSolverTypeToString(LinearSolverType);

		return oss.str();
	}
};

} // namespace fem::core
