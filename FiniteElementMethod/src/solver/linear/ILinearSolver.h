#pragma once

#include "../SolverError.h"
#include "LinearSolverResult.h"

#include "math/math.h"

#include <expected>

namespace fem::solver::linear
{

class ILinearSolver
{
public:
	virtual ~ILinearSolver() = default;

	virtual std::expected<LinearSolverResult, SolverError> Solve(const SpMat& A, const Vec& b) = 0;

	virtual std::string GetName() const = 0;
};

} // namespace fem::solver::linear
