#pragma once

#include "math/math.h"

namespace fem::solver
{

struct FEMSolverResult
{
	Vec solution;
	double totalSolverTimeMs;
	std::size_t linearSolveCount;
};

} // namespace fem::solver
