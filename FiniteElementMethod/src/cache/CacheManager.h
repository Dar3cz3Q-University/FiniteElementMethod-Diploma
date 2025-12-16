#pragma once

#include "math/math.h"

#include <string>
#include <vector>
#include <optional>
#include <Eigen/Sparse>

namespace fem::cache
{

auto constexpr CACHE_ROOT = "cache";

class CacheManager
{
public:
	struct SystemCache
	{
		SpMat H;
		SpMat C;
		Vec P;
		bool hasCapacity;
	};

	struct CacheMetadata
	{
		std::string meshFile;
		std::string configFile;
		std::string combinedHash;
		std::string meshHash;
		std::string configHash;
		size_t meshFileSize;
		size_t configFileSize;
		std::string cacheCreatedTime;
		size_t matrixHRows;
		size_t matrixHCols;
		size_t matrixHNonzeros;
		size_t matrixCRows;
		size_t matrixCCols;
		size_t matrixCNonzeros;
		size_t vectorPSize;
		bool hasCapacityMatrix;
	};

	static bool SaveSteadySystem(
		const std::string& cacheDir,
		const SpMat& H,
		const Vec& P,
		const std::string& meshFile,
		const std::string& configFile);

	static bool SaveTransientSystem(
		const std::string& cacheDir,
		const SpMat& H,
		const SpMat& C,
		const Vec& P,
		const std::string& meshFile,
		const std::string& configFile);

	static std::optional<SystemCache> LoadSystem(
		const std::string& cacheDir,
		const std::string& meshFile,
		const std::string& configFile,
		bool strictValidation = true);

	static bool IsValidCache(
		const std::string& cacheDir,
		const std::string& meshFile,
		const std::string& configFile);

	static bool ClearCache(const std::string& cacheDir);

	static void PrintCacheInfo(const std::string& cacheDir);

	static std::optional<CacheMetadata> GetMetadata(const std::string& cacheDir);

private:
	static bool SaveMetadata(const std::string& filename, const CacheMetadata& meta);
	static std::optional<CacheMetadata> LoadMetadata(const std::string& filename);
	static bool ValidateInputFiles(
		const std::string& meshFile,
		const std::string& configFile,
		const CacheMetadata& meta);
	static std::string GetCurrentTimestamp();

	CacheManager() = delete;
};

} // namespace fem::cache
