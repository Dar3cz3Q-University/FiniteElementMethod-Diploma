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

	LOG_INFO("Solver time (pure): {:.2f} ms", stats.elapsedTimeMs);
	LOG_INFO("Total time (with overhead): {:.2f} ms", totalTime);
	LOG_INFO("Overhead: {:.2f} ms", totalTime - stats.elapsedTimeMs);
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

	LOG_INFO("Total time: {:.3f} s, Time step: {:.6f} s",
		config.totalTime, config.timeStep);

	double dt = config.timeStep;
	std::size_t numSteps = static_cast<std::size_t>(config.totalTime / dt);

	LOG_INFO("Number of time steps: {}", numSteps);
	LOG_INFO("System size: {} nodes", H.rows());

	SpMat A = H + C / dt;

	auto linearSolver = linear::LinearSolverFactory::Create(solverType);
	LOG_INFO("Using linear solver: {}", linearSolver->GetName());

	auto startTime = std::chrono::high_resolution_clock::now();

	Vec T_current = Vec::Constant(H.rows(), config.initialTemperature);
	double totalSolverTime = 0.0;

	for (int step = 0; step < numSteps; ++step)
	{
		double currentTime = step * dt;

		Vec b = P + (C / dt) * T_current;

		linear::LinearSolverStats stats;
		auto T_new = linearSolver->Solve(A, b, &stats);

		if (!T_new)
		{
			LOG_WARN("LOOOL");
			/*return std::unexpected(std::format(
				"Linear solver failed at time step {} (t={:.3f}s): {}",
				step, currentTime, T_new.error().ToString()));*/
		}

		totalSolverTime += stats.elapsedTimeMs;
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
	double totalElapsedTime = std::chrono::duration<double, std::milli>(
		endTime - startTime).count();

	LOG_INFO("Transient solved in {:.2f} s", totalElapsedTime / 1000.0);
	LOG_INFO("Total solver time: {:.2f} s", totalSolverTime / 1000.0);
	LOG_INFO("Average per time step: {:.2f} ms", totalSolverTime / numSteps);

	return FEMSolverResult{
		.solution = T_current,
		.totalSolverTimeMs = totalElapsedTime,
		.linearSolveCount = numSteps
	};
}

} // namespace fem::solver
