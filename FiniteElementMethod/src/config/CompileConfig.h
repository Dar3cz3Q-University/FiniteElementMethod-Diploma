#pragma once

#include "logger/logger.h"

#include <print>

#include <Eigen/Sparse>

namespace fem::config
{

// ============================================================================
// Solver Configuration
// ============================================================================

//#define FEM_USE_SEQUENTIAL_SOLVER // Uncomment for sequential solver

// ============================================================================
// Storage Format Configuration
// ============================================================================

//#define FEM_USE_ROW_MAJOR // Uncomment for CSR format

#ifdef FEM_USE_ROW_MAJOR
constexpr int StorageOrder = Eigen::RowMajor;
constexpr const char* StorageOrderName = "CSR (RowMajor)";
#else
constexpr int StorageOrder = Eigen::ColMajor;
constexpr const char* StorageOrderName = "CSC (ColMajor)";
#endif

// ============================================================================
// Reordering Configuration (only for sequential solver)
// ============================================================================

//#define FEM_USE_NATURAL_ORDERING // No reordering. Requires FEM_USE_SEQUENTIAL_SOLVER
//#define FEM_USE_COLAMD_ORDERING  // COLAMD reordering. Requires FEM_USE_SEQUENTIAL_SOLVER

#if (defined(FEM_USE_NATURAL_ORDERING) || defined(FEM_USE_COLAMD_ORDERING)) && !defined(FEM_USE_SEQUENTIAL_SOLVER)
#error "Reordering options require FEM_USE_SEQUENTIAL_SOLVER to be defined"
#endif

#ifdef FEM_USE_SEQUENTIAL_SOLVER

#if defined(FEM_USE_NATURAL_ORDERING)
using DefaultOrderingType = Eigen::NaturalOrdering<int>;
constexpr const char* ReorderingName = "Natural (No Reordering)";
#elif defined(FEM_USE_COLAMD_ORDERING)
using DefaultOrderingType = Eigen::COLAMDOrdering<int>;
constexpr const char* ReorderingName = "COLAMD";
#else
using DefaultOrderingType = Eigen::AMDOrdering<int>;
constexpr const char* ReorderingName = "AMD";
#endif

constexpr bool UseSequentialSolver = true;

#else

using DefaultOrderingType = Eigen::AMDOrdering<int>;
constexpr const char* ReorderingName = "Pardiso Internal";
constexpr bool UseSequentialSolver = false;

#endif

// ============================================================================
// Utility Functions
// ============================================================================

inline void PrintCompileConfig()
{
	std::println("Compile Configuration:");
	std::println("  Precision: double (64-bit)");
	std::println("  Storage Format: {}", StorageOrderName);
	std::println("  Reordering: {}", ReorderingName);

#ifdef NDEBUG
	std::println("  Build Type: Release");
#else
	std::println("  Build Type: Debug");
#endif

#ifdef _OPENMP
	std::println("  OpenMP: Enabled");
#else
	std::println("  OpenMP: Disabled");
#endif

#ifdef EIGEN_USE_MKL_ALL
	std::println("  MKL: Enabled");
#else
	std::println("  MKL: Disabled");
#endif

#ifdef EIGEN_VECTORIZE
	#ifdef EIGEN_VECTORIZE_AVX512
		std::println("  SIMD: AVX-512");
	#elif defined(EIGEN_VECTORIZE_AVX2)
		std::println("  SIMD: AVX2");
	#elif defined(EIGEN_VECTORIZE_AVX)
		std::println("  SIMD: AVX");
	#elif defined(EIGEN_VECTORIZE_SSE4_2)
		std::println("  SIMD: SSE4.2");
	#elif defined(EIGEN_VECTORIZE_SSE2)
		std::println("  SIMD: SSE2");
	#else
		std::println("  SIMD: Enabled");
	#endif
#else
	std::println("  SIMD: Disabled");
#endif
}

} // namespace fem::config
