#pragma once

#include "math/math.h"
#include "FEMSolverStats.h"

#include <variant>

namespace fem::solver
{

struct SteadySolution
{
	Vec solution;
};

struct TransientSolution
{
	Vec finalSolution;
	std::vector<Vec> temperatures;
	std::vector<double> timeSteps;
	size_t saveStride;
};

struct FEMSolverResult
{
	std::variant<SteadySolution, TransientSolution> solution;
	FEMSolverStats stats;

	const Vec& getFinalSolution() const
	{
		if (isSteady())
		{
			return getSteady().solution;
		}
		else
		{
			return getTransient().finalSolution;
		}
	}

	const SteadySolution& getSteady() const
	{
		return std::get<SteadySolution>(solution);
	}

	const TransientSolution& getTransient() const
	{
		return std::get<TransientSolution>(solution);
	}

	bool isSteady() const
	{
		return std::holds_alternative<SteadySolution>(solution);
	}

	bool isTransient() const
	{
		return std::holds_alternative<TransientSolution>(solution);
	}
};

} // namespace fem::solver
