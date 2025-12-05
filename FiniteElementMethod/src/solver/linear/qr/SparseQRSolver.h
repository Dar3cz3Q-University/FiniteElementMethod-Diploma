#pragma once

#include "../ILinearSolver.h"

#include "math/math.h"

namespace fem::solver::linear
{

/// <summary>
/// Sparse QR decomposition solver.
/// Most numerically stable direct solver.
/// Good for ill-conditioned or rank-deficient systems.
/// Slower than LU and Cholesky.
/// </summary>
class SparseQRSolver : public ILinearSolver
{
public:
	std::expected<Vec, SolverError> Solve(const SpMat& A, const Vec& b, LinearSolverStats* stats = nullptr) override;

	std::string GetName() const override
	{
		return "SparseQR";
	}
};

} // namespace fem::solver::linear
