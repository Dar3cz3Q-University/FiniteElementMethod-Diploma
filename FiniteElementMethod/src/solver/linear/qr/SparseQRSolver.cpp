#include "SparseQRSolver.h"

#include "config/config.h"
#include "metrics/metrics.h"

#include <chrono>

namespace fem::solver::linear
{

std::expected<Vec, SolverError> SparseQRSolver::Solve(
	const SpMat& A,
	const Vec& b,
	LinearSolverStats* stats)
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

	size_t memBefore = metrics::MemoryMonitor::GetCurrentUsage();
	auto factorStart = std::chrono::high_resolution_clock::now();

	Eigen::SparseQR<SpMat, config::DefaultOrderingType> solver;
	solver.compute(A);

	auto factorEnd = std::chrono::high_resolution_clock::now();

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::SingularMatrix,
				"QR decomposition failed - matrix is rank deficient"
			}
		);
	}

	auto solveStart = std::chrono::high_resolution_clock::now();

	Vec x = solver.solve(b);

	auto solveEnd = std::chrono::high_resolution_clock::now();

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::NumericalInstability,
				"QR solve failed"
			}
		);
	}

	size_t memAfter = metrics::MemoryMonitor::GetCurrentUsage();
	size_t peakMem = metrics::MemoryMonitor::GetPeakUsage();

	if (stats)
	{
		stats->factorizationTimeMs = std::chrono::duration<double, std::milli>(factorEnd - factorStart).count();
		stats->solveTimeMs = std::chrono::duration<double, std::milli>(solveEnd - solveStart).count();
		stats->elapsedTimeMs = stats->factorizationTimeMs + stats->solveTimeMs;

		stats->residualNorm = (A * x - b).norm();

		stats->memoryUsedBytes = memAfter - memBefore;
		stats->peakMemoryBytes = peakMem;
	}

	return x;
}

} // namespace fem::solver::linear
