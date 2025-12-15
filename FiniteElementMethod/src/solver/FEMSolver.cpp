#include "FEMSolver.h"

#include "metrics/metrics.h"

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
	LOG_INFO("Solving Steady - State Problem");

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

	LOG_INFO("Steady Analysis Complete:");
	LOG_INFO("  Factorization time: {:.2f} ms", stats.factorizationTimeMs);
	LOG_INFO("  Solve time:         {:.2f} ms", stats.solveTimeMs);
	LOG_INFO("  Total solver time:  {:.2f} ms", stats.elapsedTimeMs);
	LOG_INFO("  Overhead:           {:.2f} ms", totalTime - stats.elapsedTimeMs);
	LOG_INFO("  Memory used:        {:.2f} MB", stats.getMemoryUsedMB());
	LOG_INFO("  Peak memory:        {:.2f} MB", stats.getPeakMemoryMB());
	LOG_INFO("  Residual norm:      {:.2e}", stats.residualNorm);
	LOG_INFO("  Solution range:     T_min = {:.2f} K, T_max = {:.2f} K", solution->minCoeff(), solution->maxCoeff());

	return FEMSolverResult{
		.solution = *solution,
		.totalSolverTimeMs = totalTime,
		.linearSolveCount = 1
	};
}

std::expected<FEMSolverResult, SolverError> FEMSolver::SolveTransient(const SpMat& H, const SpMat& C, const Vec& P, const domain::model::TransientConfig& config, linear::LinearSolverType solverType)
{
	LOG_INFO("Solving Transient Problem");

	if (H.rows() != H.cols() || C.rows() != C.cols())
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				"Matrices H and C must be square"
			}
		);

	if (H.rows() != C.rows())
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				std::format("Matrices H ({}) and C ({}) must have same dimensions", H.rows(), C.rows())
			}
		);

	if (H.rows() != P.size())
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				std::format("Matrix size ({}) doesn't match vector P size ({})", H.rows(), P.size())
			});

	if (config.timeStep <= 0.0 || config.totalTime <= 0.0)
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				std::format("Time step and total time must be positive")
			});

	double dt = config.timeStep;
	std::size_t numSteps = static_cast<std::size_t>(config.totalTime / dt);

	SpMat A = H + C / dt;

	auto linearSolver = linear::LinearSolverFactory::Create(solverType);

	LOG_INFO("Configuration:");
	LOG_INFO("  Total time:       {:.3f} s", config.totalTime);
	LOG_INFO("  Time step:        {:.6f} s", config.timeStep);
	LOG_INFO("  Number of steps:  {}", numSteps);
	LOG_INFO("  System size:      {} nodes", H.rows());
	LOG_INFO("  Initial temp:     {:.2f} K", config.initialTemperature);
	LOG_INFO("  Linear solver:    {}", linearSolver->GetName());

	size_t memBefore = metrics::MemoryMonitor::GetCurrentUsage();
	auto startTime = std::chrono::high_resolution_clock::now();

	Vec T_current = Vec::Constant(H.rows(), config.initialTemperature);

	double totalSolverTime = 0.0;
	double totalFactorizationTime = 0.0;
	double totalSolveTime = 0.0;
	double minResidual = std::numeric_limits<double>::max();
	double maxResidual = 0.0;
	size_t peakMemoryAtStep = 0;

	for (int step = 0; step < numSteps; ++step)
	{
		double currentTime = step * dt;

		Vec b = P + (C / dt) * T_current;

		linear::LinearSolverStats stats;
		auto T_new = linearSolver->Solve(A, b, &stats);

		if (!T_new)
		{
			return std::unexpected(T_new.error());
		}

		totalSolverTime += stats.elapsedTimeMs;
		totalFactorizationTime += stats.factorizationTimeMs;
		totalSolveTime += stats.solveTimeMs;
		minResidual = std::min(minResidual, stats.residualNorm);
		maxResidual = std::max(maxResidual, stats.residualNorm);

		T_current = *T_new;

		if (step % 100 == 0 || step == numSteps - 1)
		{
			const double pct = 100.0 * static_cast<double>(step + 1) / static_cast<double>(numSteps);
			LOG_INFO("Solving... {:.1f}% ({}/{})", pct, step + 1, numSteps);

			LOG_TRACE(
				"Step {}/{} (t={:.3f}s), solve: {:.2f}ms, residual: {:.2e}, T_min/max: {:.2f}/{:.2f} K",
				step + 1,
				numSteps,
				currentTime + dt,
				stats.elapsedTimeMs,
				stats.residualNorm,
				T_current.minCoeff(),
				T_current.maxCoeff()
			);
		}
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	double totalElapsedTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();

	size_t memAfter = metrics::MemoryMonitor::GetCurrentUsage();
	size_t peakMem = metrics::MemoryMonitor::GetPeakUsage();

	LOG_INFO("Transient Analysis Complete:");
	LOG_INFO("  Total wall time:        {:.2f} s", totalElapsedTime / 1000.0);
	LOG_INFO("  Total solver time:      {:.2f} s", totalSolverTime / 1000.0);
	LOG_INFO("  Avg factorization:      {:.2f} ms/step", totalFactorizationTime / numSteps);
	LOG_INFO("  Avg solve:              {:.2f} ms/step", totalSolveTime / numSteps);
	LOG_INFO("  Avg per time step:      {:.2f} ms", totalSolverTime / numSteps);
	LOG_INFO("  Memory used:            {:.2f} MB", (memAfter - memBefore) / (1024.0 * 1024.0));
	LOG_INFO("  Peak memory:            {:.2f} MB", peakMem / (1024.0 * 1024.0));
	LOG_INFO("  Residual range:         [{:.2e}, {:.2e}]", minResidual, maxResidual);
	LOG_INFO("  Final temperature:      T_min = {:.2f} K, T_max = {:.2f} K", T_current.minCoeff(), T_current.maxCoeff());

	return FEMSolverResult{
		.solution = T_current,
		.totalSolverTimeMs = totalElapsedTime,
		.linearSolveCount = numSteps
	};
}

} // namespace fem::solver
