#pragma once

#include "FEMSolverConfig.h"
#include "FEMSolverResult.h"
#include "SolverError.h"

#include "domain/domain.h"
#include "linear/linear.h"
#include "math/math.h"

namespace fem::solver
{

class FEMSolver
{
public:
	static std::expected<FEMSolverResult, SolverError> Solve(const SpMat& H, const SpMat& C, const Vec& P, const FEMSolverConfig& config);

private:
	static std::expected<FEMSolverResult, SolverError> SolveSteady(const SpMat& H, const Vec& P, linear::LinearSolverType solverType);
	static std::expected<FEMSolverResult, SolverError> SolveTransient(const SpMat& H, const SpMat& C, const Vec& P, const domain::model::TransientConfig& config, linear::LinearSolverType solverType);
};

} // namespace fem::solver
