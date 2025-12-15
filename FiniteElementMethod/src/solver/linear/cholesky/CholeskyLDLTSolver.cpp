#include "CholeskyLDLTSolver.h"

#include "config/CompileConfig.h"
#include "metrics/metrics.h"

#include <chrono>

#include <Eigen/SparseCholesky>

namespace fem::solver::linear
{

std::expected<Vec, SolverError> CholeskyLDLTSolver::Solve(const SpMat& A, const Vec& b, LinearSolverStats* stats)
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

	size_t memBefore = metrics::MemoryMonitor::GetCurrentUsage();
	auto totalStart = std::chrono::high_resolution_clock::now();

	Eigen::SimplicialLDLT<SpMat, Eigen::Lower, config::DefaultOrderingType> solver;
	solver.compute(A);

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

	Vec x = solver.solve(b);

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

	auto end = std::chrono::high_resolution_clock::now();

	if (stats)
	{
		stats->elapsedTimeMs = std::chrono::duration<double, std::milli>(end - totalStart).count();
		stats->residualNorm = (A * x - b).norm();
	}

	return x;
}

} // namespace fem::solver::linear
