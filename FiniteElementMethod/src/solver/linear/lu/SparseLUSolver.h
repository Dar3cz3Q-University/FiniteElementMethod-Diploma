#pragma once

#include "../ILinearSolver.h"

#include "math/math.h"

namespace fem::solver::linear
{

/// <summary>
/// Sparse LU decomposition solver.
/// General purpose solver for any non-singular matrix.
/// Works for both symmetric and non-symmetric systems.
/// </summary>
class SparseLUSolver : public ILinearSolver
{
public:
	std::expected<Vec, SolverError> Solve(const SpMat& A, const Vec& b, LinearSolverStats* stats = nullptr) override;

	std::string GetName() const override
	{
		return "SparseLU";
	}
};

} // namespace fem::solver::linear
