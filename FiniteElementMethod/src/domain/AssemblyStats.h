#pragma once

namespace fem::domain
{

struct AssemblyStats
{
	double elementAssemblyTimeMs = 0.0;
	double boundaryAssemblyTimeMs = 0.0;
	double tripletToSparseTimeMs = 0.0;
	double totalAssemblyTimeMs = 0.0;

	double getComputationTimeMs() const
	{
		return elementAssemblyTimeMs + boundaryAssemblyTimeMs;
	}

	double getOverheadMs() const
	{
		return totalAssemblyTimeMs - getComputationTimeMs() - tripletToSparseTimeMs;
	}

	double getOverheadPercent() const
	{
		if (totalAssemblyTimeMs == 0.0) return 0.0;
		return 100.0 * getOverheadMs() / totalAssemblyTimeMs;
	}
};

} // namespace fem::domain
