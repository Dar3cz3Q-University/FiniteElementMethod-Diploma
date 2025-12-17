#pragma once

#include "domain/AssemblyStats.h"
#include "solver/FEMSolverStats.h"

#include <optional>
#include <string>

namespace fem::fileio
{

struct FullMetrics
{
	std::string solverName;
	solver::FEMSolverStats solverStats;
	std::optional<domain::AssemblyStats> assemblyStats;
};

} // namespace fem::fileio
