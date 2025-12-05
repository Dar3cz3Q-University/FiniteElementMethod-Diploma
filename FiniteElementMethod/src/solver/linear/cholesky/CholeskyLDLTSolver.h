#pragma once

#include "../ILinearSolver.h"

#include "math/math.h"

namespace fem::solver::linear
{

/// <summary>
/// Cholesky LDLT decomposition solver.
/// Best for symmetric positive-definite matrices (typical in FEM).
/// Fastest direct solver for SPD systems.
/// </summary>
class CholeskyLDLTSolver : public ILinearSolver
{
public:
	std::expected<Vec, SolverError> Solve(const SpMat& A, const Vec& b, LinearSolverStats* stats = nullptr) override;

	std::string GetName() const override
	{
		return "Cholesky (SimplicialLDLT)";
	}
};

} // namespace fem::solver::linear
