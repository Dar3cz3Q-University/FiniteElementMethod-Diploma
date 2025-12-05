#include "FEMSolver.h"

#include <chrono>

namespace fem::solver
{

std::expected<FEMSolverResult, SolverError> FEMSolver::Solve(const SpMat& H, const SpMat& C, const Vec& P, const FEMSolverConfig& config)
{
	using enum domain::model::ProblemType;

	switch (config.problemType)
	{
	case Steady:
		return SolveSteady(H, P, config.linearSolver);

	case Transient:
		if (!config.transientConfig)
			return std::unexpected(
				SolverError{
					SolverErrorCode::InvalidInput,
					"Transient config is required for transient problems"
				}

			);

		return SolveTransient(H, C, P, *config.transientConfig, config.linearSolver);
	}

	return std::unexpected(
		SolverError{
			SolverErrorCode::InvalidInput,
			"Unknown problem type"
		}
	);
}

std::expected<FEMSolverResult, SolverError> fem::solver::FEMSolver::SolveSteady(const SpMat& H, const Vec& P, linear::LinearSolverType solverType)
{
	if (H.rows() != H.cols())
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				"Matrix H must be square"
			}
		);

	if (H.rows() != P.size())
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				std::format("Matrix H size ({}) doesn't match vector P size ({})", H.rows(), P.size())
			}
		);

	auto linearSolver = linear::LinearSolverFactory::Create(solverType);

	auto startTime = std::chrono::high_resolution_clock::now();

	linear::LinearSolverStats stats;
	auto solution = linearSolver->Solve(H, P, &stats);

	if (!solution)
		return std::unexpected(solution.error());

	auto endTime = std::chrono::high_resolution_clock::now();
	double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();

	LOG_INFO("Steady-state solved in {:.2f} ms", totalTime);
	LOG_INFO("Residual norm: {:.2e}", stats.residualNorm);
	LOG_INFO("Solution T_min = {:.2f} K, T_max = {:.2f} K", solution->minCoeff(), solution->maxCoeff());

	return FEMSolverResult{
		.solution = *solution,
		.totalSolverTimeMs = totalTime,
		.linearSolveCount = 1
	};
}

std::expected<FEMSolverResult, SolverError> FEMSolver::SolveTransient(const SpMat& H, const SpMat& C, const Vec& P, const domain::model::TransientConfig& config, linear::LinearSolverType solverType)
{
	return std::expected<FEMSolverResult, SolverError>();
}

} // namespace fem::solver
