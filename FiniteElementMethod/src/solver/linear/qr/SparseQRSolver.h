#pragma once

#include "../ILinearSolver.h"

#include "math/math.h"

namespace fem::solver::linear
{

class SparseQRSolver : public ILinearSolver
{
public:
	std::expected<LinearSolverResult, SolverError> Solve(const SpMat& A, const Vec& b) override;

	std::string GetName() const override
	{
		return "SparseQR";
	}
};

} // namespace fem::solver::linear
