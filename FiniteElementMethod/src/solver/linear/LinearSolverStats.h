#pragma once

namespace fem::solver::linear
{

struct LinearSolverStats
{
	double elapsedTimeMs = 0.0;
	double factorizationTimeMs = 0.0;
	double solveTimeMs = 0.0;

	double residualNorm = 0.0;

	size_t memoryUsedBytes = 0;
	size_t peakMemoryBytes = 0;

	size_t matrixSize = 0;
	size_t matrixNonZeros = 0;

	double getMemoryUsedMB() const
	{
		return memoryUsedBytes / (1024.0 * 1024.0);
	}

	double getPeakMemoryMB() const
	{
		return peakMemoryBytes / (1024.0 * 1024.0);
	}
};

} // namespace fem::solver::linear
