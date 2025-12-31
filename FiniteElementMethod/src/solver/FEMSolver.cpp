#include "FEMSolver.h"

#include "metrics/metrics.h"
#include "utils/utils.h"

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

	auto startTime = Now();
	auto result = linearSolver->Solve(H, P);
	auto endTime = Now();

	if (!result)
		return std::unexpected(result.error());

	double totalTime = ElapsedMs(startTime, endTime);

	const auto& solution = result->solution;
	const auto& stats = result->stats;

	LOG_INFO("Steady Analysis Complete:");
	LOG_INFO("  Total solver time:  {:.2f} ms", stats.elapsedTimeMs);
	LOG_INFO("  Factorization time: {:.2f} ms", stats.factorizationTimeMs);
	LOG_INFO("  Solve time:         {:.2f} ms", stats.solveTimeMs);
	LOG_INFO("  Overhead:           {:.2f} ms", totalTime - stats.elapsedTimeMs);
	LOG_INFO("  Memory used:        {:.2f} MB", stats.getMemoryUsedMB());
	LOG_INFO("  Peak memory:        {:.2f} MB", stats.getPeakMemoryMB());
	LOG_INFO("  Residual norm:      {:.2e}", stats.residualNorm);
	LOG_INFO("  Solution range:     T_min = {:.2f} K, T_max = {:.2f} K", solution.minCoeff(), solution.maxCoeff());  // TODO: Kelvin or else?

	return FEMSolverResult{
		.solution = SteadySolution{
			.solution = std::move(result->solution)
		},
		.stats = FEMSolverStats::FromLinearSolverStats(stats, totalTime),
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

	bool saveHistory = config.saveHistory;
	size_t saveStride = 0;

	if (saveHistory)
	{
		if (!config.saveStride.has_value())
			return std::unexpected(SolverError{
					SolverErrorCode::InvalidInput,
					"Save stride must be provided when save history is true"
				});

		saveStride = *config.saveStride;

		if (saveStride == 0)
			return std::unexpected(SolverError{
					SolverErrorCode::InvalidInput,
					"Save stride must be positive (non-zero)"
				});
	}

	double dt = config.timeStep;
	std::size_t numSteps = static_cast<std::size_t>(config.totalTime / dt);
	size_t progressStep = std::max<size_t>(1, numSteps / 10);

	LOG_INFO("Configuration:");
	LOG_INFO("  Total time:       {:.3f} s", config.totalTime);
	LOG_INFO("  Time step:        {:.6f} s", config.timeStep);
	LOG_INFO("  Number of steps:  {}", numSteps);
	LOG_INFO("  System size:      {} nodes", H.rows());
	LOG_INFO("  Initial temp:     {:.2f} K", config.initialTemperature); // TODO: Kelvin or else?

	if (saveHistory)
		LOG_INFO("  Save stride:      every {} steps", saveStride);

	auto linearSolver = linear::LinearSolverFactory::Create(solverType);
	LOG_INFO("  Linear solver:    {}", linearSolver->GetName());

	auto setupStart = Now();

	SpMat A = H + C / dt;
	Vec T_current = Vec::Constant(H.rows(), config.initialTemperature);

	auto setupEnd = Now();
	double setupTime = ElapsedMs(setupStart, setupEnd);

	LOG_INFO("  Setup time:       {:.2f} ms", setupTime);

	std::vector<Vec> temperatures;
	std::vector<double> timeSteps;

	if (saveHistory)
	{
		size_t estimatedSize = numSteps / saveStride + 2;
		temperatures.reserve(estimatedSize);
		timeSteps.reserve(estimatedSize);

		temperatures.push_back(T_current);
		timeSteps.push_back(0.0);
	}

	auto totalStart = Now();

	double totalSolverTime = 0.0;
	double totalFactorizationTime = 0.0;
	double totalSolveTime = 0.0;
	double minResidual = std::numeric_limits<double>::max();
	double maxResidual = 0.0;

	for (int step = 0; step < numSteps; ++step)
	{
		double currentTime = step * dt;

		Vec b = P + (C / dt) * T_current;

		auto result = linearSolver->Solve(A, b);

		if (!result)
			return std::unexpected(result.error());

		const auto& stats = result->stats;

		totalSolverTime += stats.elapsedTimeMs;
		totalFactorizationTime += stats.factorizationTimeMs;
		totalSolveTime += stats.solveTimeMs;
		minResidual = std::min(minResidual, stats.residualNorm);
		maxResidual = std::max(maxResidual, stats.residualNorm);

		T_current = std::move(result->solution);

		if (saveHistory && (step % saveStride == 0 || step == numSteps - 1))
		{
			temperatures.push_back(T_current);
			timeSteps.push_back(currentTime + dt);
		}

		if (step == 0 || step % progressStep == 0 || step == numSteps - 1)
		{
			double pct = 100.0 * (step + 1) / numSteps;
			LOG_INFO("Progress: {:.1f}% ({}/{}) - t = {:.3f}s, T_range = [{:.2f}, {:.2f}] K",
				pct, step + 1, numSteps, currentTime + dt,
				T_current.minCoeff(), T_current.maxCoeff());
		}
	}

	auto totalEnd = Now();
	size_t peakMem = metrics::MemoryMonitor::GetPeakUsage();

	double totalTime = ElapsedMs(totalStart, totalEnd);

	FEMSolverStats stats{
		.factorizationTimeMs = totalFactorizationTime,
		.solveTimeMs = totalSolveTime,
		.totalSolverTimeMs = totalSolverTime,
		.totalTimeMs = totalTime,
		.setupTimeMs = setupTime,
		.overheadMs = totalTime - totalSolverTime,
		.peakMemoryBytes = peakMem,
		.residualNorm = maxResidual,
		.minResidual = minResidual,
		.maxResidual = maxResidual,
		.matrixSize = static_cast<size_t>(H.rows()),
		.matrixNonZeros = static_cast<size_t>(A.nonZeros()),
		.linearSolveCount = numSteps
	};

	LOG_INFO("Transient Analysis Complete:");
	LOG_INFO("  Total time:         {:.2f} ms", stats.totalTimeMs);
	LOG_INFO("  Total solver time:  {:.2f} ms", stats.totalSolverTimeMs);
	LOG_INFO("  Loop overhead:      {:.2f} ms ({:.1f}%)", stats.overheadMs, stats.getOverheadPercent());
	LOG_INFO("  Avg factorization:  {:.2f} ms/step", stats.getAvgFactorizationMs());
	LOG_INFO("  Avg solve:          {:.2f} ms/step", stats.getAvgSolveMs());
	LOG_INFO("  Avg per step:       {:.2f} ms", stats.getAvgPerStepMs());
	LOG_INFO("  Memory used:        {:.2f} MB", stats.getMemoryUsedMB());
	LOG_INFO("  Peak memory:        {:.2f} MB", stats.getPeakMemoryMB());
	LOG_INFO("  Residual range:     [{:.2e}, {:.2e}]", stats.minResidual, stats.maxResidual);
	LOG_INFO("  Final temperature:  T_min = {:.2f} K, T_max = {:.2f} K", T_current.minCoeff(), T_current.maxCoeff());  // TODO: Kelvin or else?

	if (saveHistory)
		LOG_INFO("  History saved:      {} snapshots", temperatures.size());

	return FEMSolverResult{
		.solution = TransientSolution{
			.finalSolution = std::move(T_current),
			.temperatures = std::move(temperatures),
			.timeSteps = std::move(timeSteps),
			.saveStride = saveStride
		},
		.stats = stats
	};
}

} // namespace fem::solver
