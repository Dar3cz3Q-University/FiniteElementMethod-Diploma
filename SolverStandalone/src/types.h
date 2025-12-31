#pragma once

#include <Eigen/Sparse>
#include <chrono>

namespace fem::solver::standalone
{

using SpMat = Eigen::SparseMatrix<double, Eigen::RowMajor>;
using Vec = Eigen::VectorXd;

struct SolverStats
{
	double elapsedTimeMs = 0.0;
	double factorizationTimeMs = 0.0;
	double solveTimeMs = 0.0;

	double residualNorm = 0.0;

	size_t memoryUsedBytes = 0;
	size_t peakMemoryBytes = 0;

	size_t matrixSize = 0;
	size_t matrixNonZeros = 0;
};

inline auto Now()
{
	return std::chrono::high_resolution_clock::now();
}

inline double ElapsedMs(auto start, auto end)
{
	return std::chrono::duration<double, std::milli>(end - start).count();
}

} // namespace fem::solver::standalone
