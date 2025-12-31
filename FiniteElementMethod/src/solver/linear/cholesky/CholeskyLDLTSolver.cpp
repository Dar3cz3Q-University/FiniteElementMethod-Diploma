#include "CholeskyLDLTSolver.h"

#include "config/config.h"
#include "metrics/metrics.h"
#include "utils/utils.h"

#include <Eigen/PardisoSupport>
#include <Eigen/Sparse>

namespace fem::solver::linear
{

std::expected<LinearSolverResult, SolverError> CholeskyLDLTSolver::Solve(const SpMat& A, const Vec& b)
{
	if (A.rows() != A.cols())
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::InvalidInput,
				"Matrix must be a square"
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

#ifdef FEM_USE_SEQUENTIAL_SOLVER
	Eigen::SimplicialLDLT<SpMat, Eigen::Lower, config::DefaultOrderingType> solver;
#else
	Eigen::PardisoLDLT<SpMat> solver;
#endif

	solver.compute(A);

	auto factorEnd = Now();
	stats.factorizationTimeMs = ElapsedMs(factorStart, factorEnd);

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::SingularMatrix,
				"Cholesky decomposition failed - matrix not symmetric positive-definite"
			}
		);
	}

	auto solveStart = Now();

	Vec x = solver.solve(b);

	auto solveEnd = Now();
	stats.solveTimeMs = ElapsedMs(solveStart, solveEnd);
	stats.residualNorm = (A * x - b).norm();

	// TODO: Map Eigen errors to SolverError more precisely
	if (solver.info() != Eigen::Success)
	{
		return std::unexpected(
			SolverError{
				SolverErrorCode::NumericalInstability,
				"Cholesky solve failed"
			}
		);
	}

	stats.peakMemoryBytes = metrics::MemoryMonitor::GetPeakUsage();

	auto end = Now();
	stats.elapsedTimeMs = ElapsedMs(start, end);

	return LinearSolverResult{
		.solution = std::move(x),
		.stats = stats
	};
}

} // namespace fem::solver::linear
