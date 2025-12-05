#pragma once

#include "../SolverError.h"
#include "LinearSolverStats.h"

#include "math/math.h"

#include <expected>

namespace fem::solver::linear
{

/// <summary>
/// Interface for direct linear system solvers
/// </summary>
class ILinearSolver
{
public:
	virtual ~ILinearSolver() = default;

	/// <summary>
	/// Solve the linear system A*x = b
	/// </summary>
	/// <param name="A">System matrix (sparse)</param>
	/// <param name="b">Right-hand side vector</param>
	/// <param name="stats">Optional statistics output</param>
	/// <returns>Solution vector x or error</returns>
	virtual std::expected<Vec, SolverError> Solve(const SpMat& A, const Vec& b, LinearSolverStats* stats = nullptr) = 0;

	/// <summary>
	/// Get solver name
	/// </summary>
	virtual std::string GetName() const = 0;
};

} // namespace fem::solver::linear
