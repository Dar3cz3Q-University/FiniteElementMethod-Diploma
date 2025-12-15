#pragma once

#include "../ILinearSolver.h"

#include "math/math.h"

namespace fem::solver::linear
{

class CholeskyLDLTSolver : public ILinearSolver
{
public:
	std::expected<LinearSolverResult, SolverError> Solve(const SpMat& A, const Vec& b) override;

	std::string GetName() const override
	{
		return "Cholesky (SimplicialLDLT)";
	}
};

} // namespace fem::solver::linear
