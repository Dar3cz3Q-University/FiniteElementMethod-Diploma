#include "pardiso_solver.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <Eigen/PardisoSupport>
#include <mkl.h>

namespace fem::solver::standalone
{

	static size_t GetPeakMemory()
	{
		std::ifstream status("/proc/self/status");
		std::string line;
		while (std::getline(status, line))
		{
			if (line.compare(0, 7, "VmPeak:") == 0)
			{
				size_t kb;
				std::istringstream(line.substr(7)) >> kb;
				return kb * 1024;
			}
		}
	}

	bool SolvePARDISO_LDLT(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats)
	{
		if (K.rows() != K.cols() || K.rows() != b.size())
		{
			std::cerr << "Dimension mismatch" << std::endl;
			return false;
		}

		stats.matrixSize = K.rows();
		stats.matrixNonZeros = K.nonZeros();

		Eigen::PardisoLDLT<SpMat> solver;

		auto totalStart = Now();

		auto t1 = Now();
		solver.compute(K);
		stats.factorizationTimeMs = ElapsedMs(t1, Now());

		if (solver.info() != Eigen::Success)
		{
			std::cerr << "Compute failed" << std::endl;
			return false;
		}

		t1 = Now();
		x = solver.solve(b);
		stats.solveTimeMs = ElapsedMs(t1, Now());

		if (solver.info() != Eigen::Success)
		{
			std::cerr << "Solve failed" << std::endl;
			return false;
		}

		stats.elapsedTimeMs = ElapsedMs(totalStart, Now());
		stats.residualNorm = (K * x - b).norm();
		stats.peakMemoryBytes = GetPeakMemory();

		return true;
	}

	bool SolvePARDISO_LU(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats)
	{
		if (K.rows() != K.cols() || K.rows() != b.size())
		{
			std::cerr << "Dimension mismatch" << std::endl;
			return false;
		}

		stats.matrixSize = K.rows();
		stats.matrixNonZeros = K.nonZeros();

		Eigen::PardisoLU<SpMat> solver;

		auto totalStart = Now();

		auto t1 = Now();
		solver.compute(K);
		stats.factorizationTimeMs = ElapsedMs(t1, Now());

		if (solver.info() != Eigen::Success)
		{
			std::cerr << "Compute failed" << std::endl;
			return false;
		}

		t1 = Now();
		x = solver.solve(b);
		stats.solveTimeMs = ElapsedMs(t1, Now());

		if (solver.info() != Eigen::Success)
		{
			std::cerr << "Solve failed" << std::endl;
			return false;
		}

		stats.elapsedTimeMs = ElapsedMs(totalStart, Now());
		stats.residualNorm = (K * x - b).norm();
		stats.peakMemoryBytes = GetPeakMemory();

		return true;
	}

	void PrintBenchmark(const SolverStats& stats)
	{
		std::cout << "\n======== BENCHMARK ========" << std::endl;
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Size:          " << stats.matrixSize << "x" << stats.matrixSize << std::endl;
		std::cout << "Non-zeros:     " << stats.matrixNonZeros << std::endl;
		std::cout << "Factorization: " << stats.factorizationTimeMs << " ms" << std::endl;
		std::cout << "Solve:         " << stats.solveTimeMs << " ms" << std::endl;
		std::cout << "Total:         " << stats.elapsedTimeMs << " ms" << std::endl;
		if (stats.peakMemoryBytes > 0)
			std::cout << "Peak memory:   " << stats.peakMemoryBytes / (1024.0 * 1024.0) << " MB" << std::endl;
		std::cout << "Residual:      " << std::scientific << stats.residualNorm << std::endl;
		std::cout << "===========================" << std::endl;
	}

	void PrintMKLInfo()
	{
		MKLVersion ver;
		mkl_get_version(&ver);
		std::cout << "MKL " << ver.MajorVersion << "." << ver.MinorVersion
			<< ", threads: " << mkl_get_max_threads() << std::endl;
	}

} // namespace fem::solver::standalone
