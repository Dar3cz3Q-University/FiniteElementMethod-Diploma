#include "pardiso_solver.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>

#include <mkl_pardiso.h>
#include <mkl_types.h>
#include <mkl.h>
#include <omp.h>

namespace fem::solver::standalone
{

	static size_t GetPeakMemory()
	{
#ifdef __linux__
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
#endif
		return 0;
	}

	// Helper: extract upper triangle from symmetric matrix in CSR format
	static void ExtractUpperTriangle(const SpMat& A,
		std::vector<MKL_INT>& ia,
		std::vector<MKL_INT>& ja,
		std::vector<double>& values)
	{
		const MKL_INT n = static_cast<MKL_INT>(A.rows());
		ia.reserve(n + 1);
		ja.reserve(A.nonZeros() / 2 + n);
		values.reserve(A.nonZeros() / 2 + n);

		ia.push_back(1); // 1-based indexing

		for (MKL_INT row = 0; row < n; ++row)
		{
			for (SpMat::InnerIterator it(A, row); it; ++it)
			{
				MKL_INT col = static_cast<MKL_INT>(it.col());
				if (col >= row) // upper triangle (including diagonal)
				{
					ja.push_back(col + 1); // 1-based
					values.push_back(it.value());
				}
			}
			ia.push_back(static_cast<MKL_INT>(ja.size() + 1));
		}
	}

	// Direct MKL PARDISO call for symmetric matrices (LDLT)
	bool SolvePARDISO_LDLT(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats)
	{
		if (K.rows() != K.cols() || K.rows() != b.size())
		{
			std::cerr << "Dimension mismatch" << std::endl;
			return false;
		}

		const MKL_INT n = static_cast<MKL_INT>(K.rows());
		const MKL_INT nrhs = 1;
		const MKL_INT mtype = -2; // Real symmetric indefinite

		stats.matrixSize = n;
		stats.matrixNonZeros = K.nonZeros();

		// Extract upper triangle for PARDISO
		std::vector<MKL_INT> ia, ja;
		std::vector<double> a;
		ExtractUpperTriangle(K, ia, ja, a);

		// PARDISO control parameters
		void* pt[64];
		MKL_INT iparm[64];
		MKL_INT maxfct = 1;
		MKL_INT mnum = 1;
		MKL_INT msglvl = 1; // Verbose output - PARDISO prints statistics
		MKL_INT error = 0;

		// Initialize pt and iparm
		std::memset(pt, 0, sizeof(pt));
		std::memset(iparm, 0, sizeof(iparm));

		// Setup iparm
		iparm[0] = 1;   // Use non-default values
		iparm[1] = 3;   // Parallel nested dissection (METIS)
		iparm[3] = 0;   // No iterative-direct algorithm
		iparm[4] = 0;   // No user fill-in reducing permutation
		iparm[5] = 0;   // Write solution into x
		iparm[7] = 2;   // Max numbers of iterative refinement steps
		iparm[9] = 13;  // Perturb pivot elements (1E-13)
		iparm[10] = 1;  // Scaling (for symmetric indefinite)
		iparm[12] = 1;  // Improved accuracy with matching
		iparm[17] = -1; // Report number of non-zeros in factors
		iparm[18] = -1; // Report GFLOPS
		iparm[23] = 1;  // Two-level factorization (parallel)
		iparm[24] = 1;  // Parallel forward/backward solve
		iparm[26] = 1;  // Matrix checker
		iparm[34] = 0;  // 1-based indexing (Fortran style)
		iparm[35] = 0;  // CSR format

		// Prepare vectors
		x.resize(n);
		std::vector<double> b_copy(b.data(), b.data() + n);

		auto totalStart = Now();

		// Phase 13: Analysis + Numerical Factorization + Solve
		MKL_INT phase = 13;
		auto t1 = Now();

		pardiso(pt, &maxfct, &mnum, &mtype, &phase,
			&n, a.data(), ia.data(), ja.data(),
			nullptr, &nrhs, iparm, &msglvl,
			b_copy.data(), x.data(), &error);

		stats.factorizationTimeMs = ElapsedMs(t1, Now());

		if (error != 0)
		{
			std::cerr << "PARDISO error (phase 13): " << error << std::endl;

			// Cleanup before returning
			phase = -1;
			pardiso(pt, &maxfct, &mnum, &mtype, &phase,
				&n, nullptr, ia.data(), ja.data(),
				nullptr, &nrhs, iparm, &msglvl,
				nullptr, nullptr, &error);

			return false;
		}

		stats.elapsedTimeMs = ElapsedMs(totalStart, Now());
		stats.solveTimeMs = 0; // Combined with factorization in phase 13

		// Cleanup: Phase -1
		phase = -1;
		pardiso(pt, &maxfct, &mnum, &mtype, &phase,
			&n, nullptr, ia.data(), ja.data(),
			nullptr, &nrhs, iparm, &msglvl,
			nullptr, nullptr, &error);

		// Compute residual
		Vec Ax = K * x;
		stats.residualNorm = (Ax - b).norm();
		stats.peakMemoryBytes = GetPeakMemory();

		return true;
	}

	// Direct MKL PARDISO call for unsymmetric matrices (LU)
	bool SolvePARDISO_LU(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats)
	{
		if (K.rows() != K.cols() || K.rows() != b.size())
		{
			std::cerr << "Dimension mismatch" << std::endl;
			return false;
		}

		const MKL_INT n = static_cast<MKL_INT>(K.rows());
		const MKL_INT nrhs = 1;
		const MKL_INT mtype = 11; // Real unsymmetric

		stats.matrixSize = n;
		stats.matrixNonZeros = K.nonZeros();

		// Convert to 1-based indexing for PARDISO
		std::vector<MKL_INT> ia(n + 1);
		std::vector<MKL_INT> ja(K.nonZeros());
		std::vector<double> a(K.valuePtr(), K.valuePtr() + K.nonZeros());

		const auto* outerPtr = K.outerIndexPtr();
		const auto* innerPtr = K.innerIndexPtr();

		for (MKL_INT i = 0; i <= n; ++i)
			ia[i] = static_cast<MKL_INT>(outerPtr[i]) + 1;

		for (MKL_INT i = 0; i < static_cast<MKL_INT>(K.nonZeros()); ++i)
			ja[i] = static_cast<MKL_INT>(innerPtr[i]) + 1;

		// PARDISO control parameters
		void* pt[64];
		MKL_INT iparm[64];
		MKL_INT maxfct = 1;
		MKL_INT mnum = 1;
		MKL_INT msglvl = 1; // Verbose output - PARDISO prints statistics
		MKL_INT error = 0;

		std::memset(pt, 0, sizeof(pt));
		std::memset(iparm, 0, sizeof(iparm));

		// Setup iparm for unsymmetric
		iparm[0] = 1;   // Use non-default values
		iparm[1] = 3;   // Parallel nested dissection (METIS)
		iparm[3] = 0;   // No iterative-direct algorithm
		iparm[4] = 0;   // No user fill-in reducing permutation
		iparm[5] = 0;   // Write solution into x
		iparm[7] = 2;   // Max numbers of iterative refinement steps
		iparm[9] = 13;  // Perturb pivot elements (1E-13)
		iparm[10] = 1;  // Scaling for unsymmetric
		iparm[12] = 1;  // Weighted matching for unsymmetric
		iparm[17] = -1; // Report number of non-zeros
		iparm[18] = -1; // Report GFLOPS
		iparm[23] = 1;  // Two-level factorization
		iparm[24] = 1;  // Parallel forward/backward solve
		iparm[26] = 1;  // Matrix checker
		iparm[34] = 0;  // 1-based indexing
		iparm[35] = 0;  // CSR format

		x.resize(n);
		std::vector<double> b_copy(b.data(), b.data() + n);

		auto totalStart = Now();

		// Phase 13: Analysis + Factorization + Solve
		MKL_INT phase = 13;
		auto t1 = Now();

		pardiso(pt, &maxfct, &mnum, &mtype, &phase,
			&n, a.data(), ia.data(), ja.data(),
			nullptr, &nrhs, iparm, &msglvl,
			b_copy.data(), x.data(), &error);

		stats.factorizationTimeMs = ElapsedMs(t1, Now());

		if (error != 0)
		{
			std::cerr << "PARDISO error (phase 13): " << error << std::endl;

			phase = -1;
			pardiso(pt, &maxfct, &mnum, &mtype, &phase,
				&n, nullptr, ia.data(), ja.data(),
				nullptr, &nrhs, iparm, &msglvl,
				nullptr, nullptr, &error);

			return false;
		}

		stats.elapsedTimeMs = ElapsedMs(totalStart, Now());
		stats.solveTimeMs = 0;

		// Cleanup
		phase = -1;
		pardiso(pt, &maxfct, &mnum, &mtype, &phase,
			&n, nullptr, ia.data(), ja.data(),
			nullptr, &nrhs, iparm, &msglvl,
			nullptr, nullptr, &error);

		Vec Ax = K * x;
		stats.residualNorm = (Ax - b).norm();
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
			<< ", threads: " << mkl_get_max_threads()
			<< ", OMP threads: " << omp_get_max_threads() << std::endl;
	}

} // namespace fem::solver::standalone
