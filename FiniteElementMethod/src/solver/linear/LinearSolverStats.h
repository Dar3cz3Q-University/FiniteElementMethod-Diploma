#pragma once

#include "utils/utils.h"

namespace fem::solver::linear
{

struct LinearSolverStats
{
	double elapsedTimeMs = 0.0;
	double factorizationTimeMs = 0.0;
	double solveTimeMs = 0.0;

	double residualNorm = 0.0;

	size_t memoryUsedBytes = 0; // TODO: Change the way it's calculated
	size_t peakMemoryBytes = 0;

	size_t matrixSize = 0;
	size_t matrixNonZeros = 0;

	double getMemoryUsedMB() const
	{
		return BytesToMiB(memoryUsedBytes);
	}

	double getPeakMemoryMB() const
	{
		return BytesToMiB(peakMemoryBytes);
	}
};

} // namespace fem::solver::linear
