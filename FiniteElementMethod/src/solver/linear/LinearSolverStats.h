#pragma once

namespace fem::solver::linear
{

struct LinearSolverStats
{
	double elapsedTimeMs = 0.0;
	double residualNorm = 0.0;
};

} // namespace fem::solver::linear
