#include "types.h"
#include "matrix_io.h"
#include "pardiso_solver.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include <mkl.h>

using namespace fem::solver::standalone;

struct Options
{
	const char* matrixFile = nullptr;
	const char* rhsFile = nullptr;
	const char* solutionFile = nullptr;
	const char* metricsFile = nullptr;
	bool useLU = false;
	int threads = 0;
};

void PrintUsage(const char* name)
{
	std::cout << "Usage: " << name << " <matrix.mtx> <rhs.txt> <solution.txt> [options]\n"
		<< "Options:\n"
		<< "  -s, --solver <ldlt|lu>   Solver type (default: ldlt)\n"
		<< "  -t, --threads <n>        Number of MKL threads (default: auto)\n"
		<< "  -m, --metrics <file>     Output metrics to file\n";
}

bool ParseArgs(int argc, char* argv[], Options& opts)
{
	if (argc < 4)
		return false;

	opts.matrixFile = argv[1];
	opts.rhsFile = argv[2];
	opts.solutionFile = argv[3];

	for (int i = 4; i < argc; ++i)
	{
		if ((std::strcmp(argv[i], "-s") == 0 || std::strcmp(argv[i], "--solver") == 0) && i + 1 < argc)
		{
			opts.useLU = std::strcmp(argv[++i], "lu") == 0;
		}
		else if ((std::strcmp(argv[i], "-t") == 0 || std::strcmp(argv[i], "--threads") == 0) && i + 1 < argc)
		{
			opts.threads = std::atoi(argv[++i]);
		}
		else if ((std::strcmp(argv[i], "-m") == 0 || std::strcmp(argv[i], "--metrics") == 0) && i + 1 < argc)
		{
			opts.metricsFile = argv[++i];
		}
	}
	return true;
}

void SaveMetrics(const char* filename, const SolverStats& stats, const Options& opts)
{
	std::ofstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Cannot write metrics to: " << filename << std::endl;
		return;
	}

	file << "Solver: " << (opts.useLU ? "PardisoLU" : "PardisoLDLT") << "\n"
		<< "Threads: " << mkl_get_max_threads() << "\n"
		<< "Matrix size: " << stats.matrixSize << "\n"
		<< "Non-zeros: " << stats.nonZeros << "\n"
		<< "Analysis time (ms): " << stats.analyzeTimeMs << "\n"
		<< "Factorization time (ms): " << stats.factorizeTimeMs << "\n"
		<< "Solve time (ms): " << stats.solveTimeMs << "\n"
		<< "Total time (ms): " << stats.totalTimeMs << "\n"
		<< "Peak memory (MB): " << stats.peakMemoryBytes / (1024.0 * 1024.0) << "\n"
		<< "Residual norm: " << stats.residualNorm << "\n";

	std::cout << "Metrics saved to: " << filename << std::endl;
}

int main(int argc, char* argv[])
{
	std::cout << "Standalone Solver" << std::endl;

	Options opts;
	if (!ParseArgs(argc, argv, opts))
	{
		PrintUsage(argv[0]);
		return 1;
	}

	if (opts.threads > 0)
		mkl_set_num_threads(opts.threads);

	PrintMKLInfo();

	std::cout << "Solver: " << (opts.useLU ? "PardisoLU" : "PardisoLDLT") << std::endl;

	SpMat K;
	Vec b, x;

	if (!LoadMatrixMarket(opts.matrixFile, K))
		return 1;
	if (!LoadVector(opts.rhsFile, b))
		return 1;

	SolverStats stats;
	bool success = opts.useLU ? SolvePARDISO_LU(K, b, x, stats) : SolvePARDISO_LDLT(K, b, x, stats);

	if (!success)
		return 1;
	if (!SaveVector(opts.solutionFile, x))
		return 1;

	PrintBenchmark(stats);

	if (opts.metricsFile)
		SaveMetrics(opts.metricsFile, stats, opts);

	return 0;
}
