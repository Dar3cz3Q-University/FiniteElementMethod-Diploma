#pragma once

#include "logger/logger.h"

#include <print>

#include <Eigen/Sparse>

namespace fem::config
{

// ============================================================================
// SPARSE MATRIX STORAGE FORMAT
// ============================================================================

//#define FEM_USE_ROW_MAJOR  // Uncomment for CSR format

#ifdef FEM_USE_ROW_MAJOR
constexpr int StorageOrder = Eigen::RowMajor;
constexpr const char* StorageOrderName = "CSR (RowMajor)";
#else
constexpr int StorageOrder = Eigen::ColMajor;
constexpr const char* StorageOrderName = "CSC (ColMajor)";
#endif

// ============================================================================
// MATRIX REORDERING STRATEGY
// ============================================================================

//#define FEM_USE_NATURAL_ORDERING  // Uncomment for no reordering
//#define FEM_USE_COLAMD_ORDERING   // Uncomment for COLAMD

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

inline void PrintCompileConfig()
{
	std::println("Compile Configuration:");
	std::println("Precision: double (64-bit)");
	std::println("Storage Format: {}", StorageOrderName);
	std::println("Reordering: {}", ReorderingName);

#ifdef NDEBUG
	std::println("Build Type: Release");
#else
	std::println("Build Type: Debug");
#endif

#ifdef _OPENMP
	std::println("OpenMP: Enabled");
#else
	std::println("OpenMP: Disabled");
#endif
}

} // namespace fem::config
