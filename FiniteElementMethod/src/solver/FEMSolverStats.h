#pragma once

#include "linear/LinearSolverStats.h"

#include "utils/utils.h"

#include <limits>

namespace fem::solver
{

struct FEMSolverStats
{
	double factorizationTimeMs = 0.0;
	double solveTimeMs = 0.0;
	double totalSolverTimeMs = 0.0;
	double totalTimeMs = 0.0;
	double setupTimeMs = 0.0;
	double overheadMs = 0.0;

	size_t peakMemoryBytes = 0;

	double residualNorm = 0.0;
	double minResidual = std::numeric_limits<double>::max();
	double maxResidual = 0.0;

	size_t matrixSize = 0;
	size_t matrixNonZeros = 0;
	size_t linearSolveCount = 1;

	double getPeakMemoryMB() const
	{
		return BytesToMiB(peakMemoryBytes);
	}

	double getOverheadPercent() const
	{
		if (totalTimeMs == 0.0) return 0.0;
		return 100.0 * overheadMs / totalTimeMs;
	}

	double getAvgFactorizationMs() const
	{
		if (linearSolveCount == 0) return 0.0;
		return factorizationTimeMs / linearSolveCount;
	}

	double getAvgSolveMs() const
	{
		if (linearSolveCount == 0) return 0.0;
		return solveTimeMs / linearSolveCount;
	}

	double getAvgPerStepMs() const
	{
		if (linearSolveCount == 0) return 0.0;
		return totalSolverTimeMs / linearSolveCount;
	}

	static FEMSolverStats FromLinearSolverStats(const linear::LinearSolverStats& linearStats, double totalTimeMs)
	{
		return FEMSolverStats{
			.factorizationTimeMs = linearStats.factorizationTimeMs,
			.solveTimeMs = linearStats.solveTimeMs,
			.totalSolverTimeMs = linearStats.elapsedTimeMs,
			.totalTimeMs = totalTimeMs,
			.setupTimeMs = 0.0,
			.overheadMs = totalTimeMs - linearStats.elapsedTimeMs,
			.peakMemoryBytes = linearStats.peakMemoryBytes,
			.residualNorm = linearStats.residualNorm,
			.minResidual = linearStats.residualNorm,
			.maxResidual = linearStats.residualNorm,
			.matrixSize = linearStats.matrixSize,
			.matrixNonZeros = linearStats.matrixNonZeros,
			.linearSolveCount = 1
		};
	}
};

}
