#include "SparseQRSolver.h"

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

	auto start = std::chrono::high_resolution_clock::now();

	Eigen::SparseQR<SpMat, Eigen::COLAMDOrdering<int>> solver;
	solver.compute(A);

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

	Vec x = solver.solve(b);

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

	auto end = std::chrono::high_resolution_clock::now();

	if (stats)
	{
		stats->elapsedTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
		stats->residualNorm = (A * x - b).norm();
	}

	return x;
}

} // namespace fem::solver::linear
