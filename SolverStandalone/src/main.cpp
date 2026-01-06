#include "types.h"
#include "matrix_io.h"
#include "pardiso_solver.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <filesystem>

#include <mkl.h>
#include <omp.h>

using namespace fem::solver::standalone;
namespace fs = std::filesystem;

struct Options
{
	fs::path directory;
	fs::path matrixFile;
	fs::path rhsFile;
	fs::path solutionFile;
	fs::path metricsFile;
	bool useLU = false;
	int threads = 0;
	bool quiet = false;
};

void PrintUsage(const char* name)
{
	std::cout << "Usage: " << name << " <directory> [options]\n"
		<< "\n"
		<< "The directory should contain:\n"
		<< "  H.mtx  - stiffness matrix (Matrix Market format)\n"
		<< "  P.txt  - load vector\n"
		<< "\n"
		<< "Output:\n"
		<< "  solution.txt - solution vector (saved in the same directory)\n"
		<< "\n"
		<< "Options:\n"
		<< "  -s, --solver <ldlt|lu>   Solver type (default: ldlt)\n"
		<< "  -t, --threads <n>        Number of MKL threads (default: auto)\n"
		<< "  -m, --metrics <file>     Output metrics to file\n"
		<< "  -q, --quiet              Suppress solver stats (only PARDISO output)\n";
}

bool ParseArgs(int argc, char* argv[], Options& opts)
{
	if (argc < 2)
		return false;

	opts.directory = argv[1];

	if (!fs::exists(opts.directory) || !fs::is_directory(opts.directory))
	{
		std::cerr << "Error: '" << opts.directory.string() << "' is not a valid directory\n";
		return false;
	}

	opts.matrixFile = opts.directory / "H.mtx";
	opts.rhsFile = opts.directory / "P.txt";
	opts.solutionFile = opts.directory / "solution.txt";

	if (!fs::exists(opts.matrixFile))
	{
		std::cerr << "Error: Matrix file not found: " << opts.matrixFile.string() << "\n";
		return false;
	}

	if (!fs::exists(opts.rhsFile))
	{
		std::cerr << "Error: RHS vector file not found: " << opts.rhsFile.string() << "\n";
		return false;
	}

	for (int i = 2; i < argc; ++i)
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
		else if (std::strcmp(argv[i], "-q") == 0 || std::strcmp(argv[i], "--quiet") == 0)
		{
			opts.quiet = true;
		}
	}
	return true;
}

void SaveMetrics(const fs::path& filename, const SolverStats& stats, const Options& opts)
{
	if (filename.has_parent_path())
		fs::create_directories(filename.parent_path());

	std::ofstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Cannot write metrics to: " << filename.string() << std::endl;
		return;
	}

	file << "Solver: " << (opts.useLU ? "PardisoLU" : "PardisoLDLT") << "\n"
		<< "Threads: " << mkl_get_max_threads() << "\n"
		<< "Matrix size: " << stats.matrixSize << "\n"
		<< "Non-zeros: " << stats.matrixNonZeros << "\n"
		<< "Analysis time (ms): " << stats.analysisTimeMs << "\n"
		<< "Factorization time (ms): " << stats.factorizationTimeMs << "\n"
		<< "Solve time (ms): " << stats.solveTimeMs << "\n"
		<< "Total time (ms): " << stats.elapsedTimeMs << "\n"
		<< "Peak memory (MB): " << stats.peakMemoryBytes / (1024.0 * 1024.0) << "\n"
		<< "Residual norm: " << stats.residualNorm << "\n";

	std::cout << "Metrics saved to: " << filename.string() << std::endl;
}

int main(int argc, char* argv[])
{
	Options opts;
	if (!ParseArgs(argc, argv, opts))
	{
		PrintUsage(argv[0]);
		return 1;
	}

	mkl_set_dynamic(0);
	if (opts.threads > 0)
	{
		mkl_set_num_threads(opts.threads);
		omp_set_num_threads(opts.threads);
	}

	if (!opts.quiet)
	{
		std::cout << "Standalone Solver\n" << std::endl;
		PrintMKLInfo();
		std::cout << "Directory: " << opts.directory.string() << std::endl;
		std::cout << "Solver: " << (opts.useLU ? "PardisoLU" : "PardisoLDLT") << std::endl;
	}

	SpMat H;
	Vec P, T;

	if (!LoadMatrixMarket(opts.matrixFile.string(), H))
		return 1;
	if (!LoadVector(opts.rhsFile.string(), P))
		return 1;

	SolverStats stats;
	bool success = opts.useLU ? SolvePARDISO_LU(H, P, T, stats) : SolvePARDISO_LDLT(H, P, T, stats);

	if (!success)
		return 1;

	std::cout << "\nTime total: " << std::fixed << std::setprecision(6)
		<< stats.elapsedTimeMs / 1000.0 << std::endl;

	if (!SaveVector(opts.solutionFile.string(), T))
		return 1;

	if (!opts.quiet)
		PrintBenchmark(stats);

	if (!opts.metricsFile.empty())
		SaveMetrics(opts.metricsFile, stats, opts);

	return 0;
}
