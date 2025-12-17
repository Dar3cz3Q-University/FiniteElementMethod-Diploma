#pragma once

#include "logger/logger.h"

#include <print>

#include <Eigen/Sparse>

namespace fem::config
{

//#define FEM_USE_SEQUENTIAL_SOLVER // Uncomment for sequential solver

//#define FEM_USE_ROW_MAJOR  // Uncomment for CSR format FEM_USE_SEQUENTIAL_SOLVER must be enabled

#ifdef FEM_USE_ROW_MAJOR
constexpr int StorageOrder = Eigen::RowMajor;
constexpr const char* StorageOrderName = "CSR (RowMajor)";
#else
constexpr int StorageOrder = Eigen::ColMajor;
constexpr const char* StorageOrderName = "CSC (ColMajor)";
#endif

//#define FEM_USE_NATURAL_ORDERING  // Uncomment for no reordering. FEM_USE_SEQUENTIAL_SOLVER must be enabled
//#define FEM_USE_COLAMD_ORDERING   // Uncomment for COLAMD FEM_USE_SEQUENTIAL_SOLVER must be enabled

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
