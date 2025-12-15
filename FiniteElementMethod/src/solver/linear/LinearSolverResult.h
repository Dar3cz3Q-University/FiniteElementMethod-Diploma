#pragma once

#include "LinearSolverStats.h"

#include "math/math.h"

namespace fem::solver::linear
{

struct LinearSolverResult
{
	Vec solution;
	LinearSolverStats stats;
};

}
