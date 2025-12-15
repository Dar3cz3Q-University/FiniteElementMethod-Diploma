#include "SparseLUSolver.h"

#include "config/config.h"
#include "metrics/metrics.h"

#include <chrono>

namespace fem::solver::linear
{

std::expected<LinearSolverResult, SolverError> SparseLUSolver::Solve(const SpMat& A, const Vec& b)
{
	if (A.rows() != A.cols())
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				"Matrix must be square"
			}
		);
	}

	if (A.rows() != b.size())
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				std::format("Matrix size ({}) doesn't match vector size ({})", A.rows(), b.size())
			}
		);
	}

	LinearSolverStats stats;
	stats.matrixSize = A.rows();
	stats.matrixNonZeros = A.nonZeros();

	size_t memBefore = metrics::MemoryMonitor::GetCurrentUsage();

	auto factorStart = metrics::Now();

	Eigen::SparseLU<SpMat, config::DefaultOrderingType> solver;
	solver.compute(A);

	auto factorEnd = metrics::Now();
	stats.factorizationTimeMs = metrics::ElapsedMs(factorStart, factorEnd);

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::SingularMatrix,
				"LU decomposition failed - matrix is singular"
			}
		);
	}

	auto solveStart = metrics::Now();

	Vec x = solver.solve(b);

	auto solveEnd = metrics::Now();
	stats.solveTimeMs = metrics::ElapsedMs(solveStart, solveEnd);

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::NumericalInstability,
				"LU solve failed"
			}
		);
	}

	size_t memAfter = metrics::MemoryMonitor::GetCurrentUsage();
	stats.memoryUsedBytes = memAfter - memBefore;
	stats.peakMemoryBytes = metrics::MemoryMonitor::GetPeakUsage();

	return LinearSolverResult{
		.solution = std::move(x),
		.stats = stats
	};
}

} // namespace fem::solver::linear
