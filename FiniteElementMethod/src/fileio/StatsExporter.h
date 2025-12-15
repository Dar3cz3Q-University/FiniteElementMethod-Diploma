#pragma once

#include "solver/solver.h"

#include <expected>
#include <filesystem>
#include <string_view>

// TODO: Create custom error type
namespace fem::fileio
{

constexpr std::string_view exportHeader =
"Solver,StorageFormat,Reordering,ProblemSize,NonZeros,"
"FactorizationMs,SolveMs,TotalSolverMs,TotalMs,OverheadMs,"
"MemoryUsedMB,PeakMemoryMB,ResidualNorm\n";

namespace fs = std::filesystem;

class StatsExporter
{
public:
	static std::expected<void, std::string> Export(const fs::path& path, const std::string_view& solverName, const solver::FEMSolverStats& stats);
};

}
