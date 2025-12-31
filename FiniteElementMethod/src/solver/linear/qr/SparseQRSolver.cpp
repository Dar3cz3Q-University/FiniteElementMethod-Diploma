#include "SparseQRSolver.h"

#include "config/config.h"
#include "metrics/metrics.h"
#include "utils/utils.h"

namespace fem::solver::linear
{

std::expected<LinearSolverResult, SolverError> SparseQRSolver::Solve(const SpMat& A, const Vec& b)
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

	auto start = Now();

	LinearSolverStats stats;
	stats.matrixSize = A.rows();
	stats.matrixNonZeros = A.nonZeros();

	auto factorStart = Now();

	Eigen::SparseQR<SpMat, config::DefaultOrderingType> solver;
	solver.compute(A);

	auto factorEnd = Now();
	stats.factorizationTimeMs = ElapsedMs(factorStart, factorEnd);

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

	auto solveStart = Now();

	Vec x = solver.solve(b);

	auto solveEnd = Now();
	stats.solveTimeMs = ElapsedMs(solveStart, solveEnd);

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

	stats.peakMemoryBytes = metrics::MemoryMonitor::GetPeakUsage();

	auto end = Now();
	stats.elapsedTimeMs = ElapsedMs(start, end);
	stats.residualNorm = (A * x - b).norm();

	return LinearSolverResult{
		.solution = std::move(x),
		.stats = stats
	};
}

} // namespace fem::solver::linear
