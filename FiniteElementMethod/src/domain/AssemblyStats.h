#pragma once

#include <cstddef>

namespace fem::domain
{

struct AssemblyStats
{
	// Timing (ms)
	double elementAssemblyTimeMs = 0.0;
	double boundaryAssemblyTimeMs = 0.0;
	double tripletToSparseTimeMs = 0.0;
	double mergeTimeMs = 0.0;
	double totalAssemblyTimeMs = 0.0;

	// Counts
	size_t elementCount = 0;
	size_t boundaryElementCount = 0;
	size_t tripletsHCount = 0;
	size_t tripletsCCount = 0;

	// Memory estimates (bytes)
	size_t tripletsMemoryBytes = 0;
	size_t sparseMatrixMemoryBytes = 0;

	double getComputationTimeMs() const
	{
		return elementAssemblyTimeMs + boundaryAssemblyTimeMs;
	}

	double getOverheadMs() const
	{
		return totalAssemblyTimeMs - getComputationTimeMs() - tripletToSparseTimeMs - mergeTimeMs;
	}

	double getOverheadPercent() const
	{
		if (totalAssemblyTimeMs == 0.0) return 0.0;
		return 100.0 * getOverheadMs() / totalAssemblyTimeMs;
	}

	double getElementsPerSecond() const
	{
		if (elementAssemblyTimeMs == 0.0) return 0.0;
		return (elementCount * 1000.0) / elementAssemblyTimeMs;
	}

	double getBoundaryElementsPerSecond() const
	{
		if (boundaryAssemblyTimeMs == 0.0) return 0.0;
		return (boundaryElementCount * 1000.0) / boundaryAssemblyTimeMs;
	}

	double getTripletsMemoryMB() const
	{
		return static_cast<double>(tripletsMemoryBytes) / (1024.0 * 1024.0);
	}

	double getSparseMatrixMemoryMB() const
	{
		return static_cast<double>(sparseMatrixMemoryBytes) / (1024.0 * 1024.0);
	}
};

} // namespace fem::domain
