#include "CacheManager.h"

#include "MatrixSerializer.h"
#include "HashUtils.h"

#include "logger/logger.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

namespace fem::cache
{

namespace fs = std::filesystem;

std::string CacheManager::GetCacheDir(const std::string& cacheRoot, const std::string& meshFile, const std::string& configFile)
{
	auto combinedHash = HashUtils::ComputeMultiFileHash({ meshFile, configFile });
	if (!combinedHash)
	{
		return "";
	}

	// Use first 16 characters of hash for directory name
	std::string shortHash = combinedHash->substr(0, 16);
	return cacheRoot + "/" + shortHash;
}

bool CacheManager::SaveSteadySystem(
	const std::string& cacheRoot,
	const SpMat& H,
	const Vec& P,
	const std::string& meshFile,
	const std::string& configFile)
{
	std::string cacheDir = GetCacheDir(cacheRoot, meshFile, configFile);
	if (cacheDir.empty())
	{
		LOG_ERROR("Failed to compute cache directory path");
		return false;
	}

	LOG_INFO("Saving steady-state system to cache: {}", cacheDir);
	fs::create_directories(cacheDir);

	auto meshHash = HashUtils::ComputeFileHash(meshFile);
	auto configHash = HashUtils::ComputeFileHash(configFile);
	auto combinedHash = HashUtils::ComputeMultiFileHash({ meshFile, configFile });

	if (!meshHash || !configHash || !combinedHash)
	{
		LOG_ERROR("Failed to compute input file hashes");
		return false;
	}

	if (!MatrixSerializer::SaveSparseMatrix(cacheDir + "/H.mtx", H))
	{
		return false;
	}

	if (!MatrixSerializer::SaveVector(cacheDir + "/P.bin", P))
	{
		return false;
	}

	CacheMetadata meta;
	meta.meshFile = meshFile;
	meta.configFile = configFile;
	meta.combinedHash = *combinedHash;
	meta.meshHash = *meshHash;
	meta.configHash = *configHash;
	meta.meshFileSize = fs::file_size(meshFile);
	meta.configFileSize = fs::file_size(configFile);
	meta.cacheCreatedTime = GetCurrentTimestamp();
	meta.matrixHRows = H.rows();
	meta.matrixHCols = H.cols();
	meta.matrixHNonzeros = H.nonZeros();
	meta.matrixCRows = 0;
	meta.matrixCCols = 0;
	meta.matrixCNonzeros = 0;
	meta.vectorPSize = P.size();
	meta.hasCapacityMatrix = false;

	if (!SaveMetadata(cacheDir + "/metadata.json", meta))
	{
		return false;
	}

	LOG_INFO("Cache saved successfully");
	LOG_INFO("  Hash: {}", meta.combinedHash.substr(0, 16));
	LOG_INFO("  Matrix H: {}x{}, {} nonzeros", meta.matrixHRows, meta.matrixHCols, meta.matrixHNonzeros);

	return true;
}

bool CacheManager::SaveTransientSystem(
	const std::string& cacheRoot,
	const SpMat& H,
	const SpMat& C,
	const Vec& P,
	const std::string& meshFile,
	const std::string& configFile)
{
	std::string cacheDir = GetCacheDir(cacheRoot, meshFile, configFile);
	if (cacheDir.empty())
	{
		LOG_ERROR("Failed to compute cache directory path");
		return false;
	}

	LOG_INFO("Saving transient system to cache: {}", cacheDir);
	fs::create_directories(cacheDir);

	auto meshHash = HashUtils::ComputeFileHash(meshFile);
	auto configHash = HashUtils::ComputeFileHash(configFile);
	auto combinedHash = HashUtils::ComputeMultiFileHash({ meshFile, configFile });

	if (!meshHash || !configHash || !combinedHash)
	{
		LOG_ERROR("Failed to compute input file hashes");
		return false;
	}

	if (!MatrixSerializer::SaveSparseMatrix(cacheDir + "/H.mtx", H))
	{
		return false;
	}

	if (!MatrixSerializer::SaveSparseMatrix(cacheDir + "/C.mtx", C))
	{
		return false;
	}

	if (!MatrixSerializer::SaveVector(cacheDir + "/P.bin", P))
	{
		return false;
	}

	CacheMetadata meta;
	meta.meshFile = meshFile;
	meta.configFile = configFile;
	meta.combinedHash = *combinedHash;
	meta.meshHash = *meshHash;
	meta.configHash = *configHash;
	meta.meshFileSize = fs::file_size(meshFile);
	meta.configFileSize = fs::file_size(configFile);
	meta.cacheCreatedTime = GetCurrentTimestamp();
	meta.matrixHRows = H.rows();
	meta.matrixHCols = H.cols();
	meta.matrixHNonzeros = H.nonZeros();
	meta.matrixCRows = C.rows();
	meta.matrixCCols = C.cols();
	meta.matrixCNonzeros = C.nonZeros();
	meta.vectorPSize = P.size();
	meta.hasCapacityMatrix = true;

	if (!SaveMetadata(cacheDir + "/metadata.json", meta))
	{
		return false;
	}

	LOG_INFO("Cache saved successfully");
	LOG_INFO("  Hash: {}", meta.combinedHash.substr(0, 16));
	LOG_INFO("  Matrix H: {}x{}, {} nonzeros", meta.matrixHRows, meta.matrixHCols, meta.matrixHNonzeros);
	LOG_INFO("  Matrix C: {}x{}, {} nonzeros", meta.matrixCRows, meta.matrixCCols, meta.matrixCNonzeros);

	return true;
}

std::optional<CacheManager::SystemCache> CacheManager::LoadSystem(
	const std::string& cacheRoot,
	const std::string& meshFile,
	const std::string& configFile,
	bool strictValidation)
{
	std::string cacheDir = GetCacheDir(cacheRoot, meshFile, configFile);
	if (cacheDir.empty())
	{
		LOG_TRACE("Failed to compute cache directory path");
		return std::nullopt;
	}

	std::string metadataPath = cacheDir + "/metadata.json";
	if (!fs::exists(metadataPath))
	{
		LOG_TRACE("Cache not found: {}", cacheDir);
		return std::nullopt;
	}

	auto meta = LoadMetadata(metadataPath);
	if (!meta)
	{
		LOG_ERROR("Failed to load cache metadata");
		return std::nullopt;
	}

	LOG_INFO("Found cache from: {}", meta->cacheCreatedTime);
	LOG_INFO("  Hash: {}", meta->combinedHash.substr(0, 16));

	if (strictValidation)
	{
		if (!ValidateInputFiles(meshFile, configFile, *meta))
		{
			LOG_ERROR("Cache validation failed - input files have changed!");
			return std::nullopt;
		}
		LOG_INFO("Cache validation passed - input files unchanged");
	}

	SystemCache cache;
	cache.hasCapacity = meta->hasCapacityMatrix;

	if (!MatrixSerializer::LoadSparseMatrix(cacheDir + "/H.mtx", cache.H))
	{
		LOG_ERROR("Failed to load matrix H");
		return std::nullopt;
	}

	if (cache.hasCapacity && !MatrixSerializer::LoadSparseMatrix(cacheDir + "/C.mtx", cache.C))
	{
		LOG_ERROR("Failed to load matrix C");
		return std::nullopt;
	}

	if (!MatrixSerializer::LoadVector(cacheDir + "/P.bin", cache.P))
	{
		LOG_ERROR("Failed to load vector P");
		return std::nullopt;
	}

	if (cache.H.rows() != meta->matrixHRows || cache.H.cols() != meta->matrixHCols)
	{
		LOG_ERROR("Matrix H dimensions mismatch!");
		return std::nullopt;
	}

	if (cache.hasCapacity && (cache.C.rows() != meta->matrixCRows || cache.C.cols() != meta->matrixCCols))
	{
		LOG_ERROR("Matrix C dimensions mismatch!");
		return std::nullopt;
	}

	if (cache.P.size() != meta->vectorPSize)
	{
		LOG_ERROR("Vector P size mismatch!");
		return std::nullopt;
	}

	LOG_INFO("System loaded from cache successfully");
	return cache;
}

bool CacheManager::IsValidCache(
	const std::string& cacheRoot,
	const std::string& meshFile,
	const std::string& configFile)
{
	std::string cacheDir = GetCacheDir(cacheRoot, meshFile, configFile);
	if (cacheDir.empty())
	{
		return false;
	}

	auto meta = LoadMetadata(cacheDir + "/metadata.json");
	if (!meta)
	{
		return false;
	}

	return ValidateInputFiles(meshFile, configFile, *meta);
}

bool CacheManager::ClearCache(const std::string& cacheRoot)
{
	if (!fs::exists(cacheRoot))
	{
		return true;
	}

	try
	{
		fs::remove_all(cacheRoot);
		LOG_INFO("Cache cleared: {}", cacheRoot);
		return true;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to clear cache: {}", e.what());
		return false;
	}
}

bool CacheManager::ClearAllCaches(const std::string& cacheRoot)
{
	if (!fs::exists(cacheRoot))
	{
		LOG_INFO("No cache directory found");
		return true;
	}

	try
	{
		size_t count = 0;
		for (const auto& entry : fs::directory_iterator(cacheRoot))
		{
			if (entry.is_directory())
			{
				fs::remove_all(entry.path());
				count++;
			}
		}
		LOG_INFO("Cleared {} cache entries from: {}", count, cacheRoot);
		return true;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to clear caches: {}", e.what());
		return false;
	}
}

void CacheManager::PrintCacheInfo(const std::string& cacheRoot, const std::string& meshFile, const std::string& configFile)
{
	std::string cacheDir = GetCacheDir(cacheRoot, meshFile, configFile);
	if (cacheDir.empty())
	{
		LOG_INFO("Failed to compute cache directory path");
		return;
	}

	auto meta = GetMetadata(cacheDir);
	if (!meta)
	{
		LOG_INFO("No valid cache found at: {}", cacheDir);
		return;
	}

	LOG_INFO("Cache Information:");
	LOG_INFO("  Location: {}", cacheDir);
	LOG_INFO("  Created: {}", meta->cacheCreatedTime);
	LOG_INFO("  Input Files:");
	LOG_INFO("    Mesh:   {} ({:.2f} MB)", meta->meshFile, meta->meshFileSize / (1024.0 * 1024.0));
	LOG_INFO("    Config: {} ({:.2f} KB)", meta->configFile, meta->configFileSize / 1024.0);
	LOG_INFO("  Hashes:");
	LOG_INFO("    Mesh:     {}", meta->meshHash);
	LOG_INFO("    Config:   {}", meta->configHash);
	LOG_INFO("    Combined: {}", meta->combinedHash);
	LOG_INFO("  System:");
	LOG_INFO("    Matrix H: {}x{}, {} nonzeros",
		meta->matrixHRows, meta->matrixHCols, meta->matrixHNonzeros);
	if (meta->hasCapacityMatrix)
	{
		LOG_INFO("    Matrix C: {}x{}, {} nonzeros",
			meta->matrixCRows, meta->matrixCCols, meta->matrixCNonzeros);
	}
	LOG_INFO("    Vector P: {} elements", meta->vectorPSize);

	size_t cacheSize = 0;
	for (const auto& entry : fs::recursive_directory_iterator(cacheDir))
	{
		if (entry.is_regular_file())
		{
			cacheSize += entry.file_size();
		}
	}
	LOG_INFO("  Cache size: {:.2f} MB", cacheSize / (1024.0 * 1024.0));
}

void CacheManager::ListCaches(const std::string& cacheRoot)
{
	if (!fs::exists(cacheRoot))
	{
		LOG_INFO("No cache directory found at: {}", cacheRoot);
		return;
	}

	LOG_INFO("Cached systems in {}:", cacheRoot);

	size_t totalSize = 0;
	size_t count = 0;

	for (const auto& entry : fs::directory_iterator(cacheRoot))
	{
		if (!entry.is_directory()) continue;

		auto meta = GetMetadata(entry.path().string());
		if (!meta) continue;

		size_t dirSize = 0;
		for (const auto& file : fs::recursive_directory_iterator(entry.path()))
		{
			if (file.is_regular_file())
			{
				dirSize += file.file_size();
			}
		}

		LOG_INFO("  [{}] {} - {}x{} matrix, {:.2f} MB",
			entry.path().filename().string(),
			meta->cacheCreatedTime,
			meta->matrixHRows, meta->matrixHCols,
			dirSize / (1024.0 * 1024.0));

		totalSize += dirSize;
		count++;
	}

	LOG_INFO("Total: {} caches, {:.2f} MB", count, totalSize / (1024.0 * 1024.0));
}

std::optional<CacheManager::CacheMetadata> CacheManager::GetMetadata(const std::string& cacheDir)
{
	return LoadMetadata(cacheDir + "/metadata.json");
}

bool CacheManager::SaveMetadata(const std::string& filename, const CacheMetadata& meta)
{
	nlohmann::json j;
	j["mesh_file"] = meta.meshFile;
	j["config_file"] = meta.configFile;
	j["combined_hash"] = meta.combinedHash;
	j["mesh_hash"] = meta.meshHash;
	j["config_hash"] = meta.configHash;
	j["mesh_file_size"] = meta.meshFileSize;
	j["config_file_size"] = meta.configFileSize;
	j["cache_created_time"] = meta.cacheCreatedTime;
	j["matrix_h_rows"] = meta.matrixHRows;
	j["matrix_h_cols"] = meta.matrixHCols;
	j["matrix_h_nonzeros"] = meta.matrixHNonzeros;
	j["matrix_c_rows"] = meta.matrixCRows;
	j["matrix_c_cols"] = meta.matrixCCols;
	j["matrix_c_nonzeros"] = meta.matrixCNonzeros;
	j["vector_p_size"] = meta.vectorPSize;
	j["has_capacity_matrix"] = meta.hasCapacityMatrix;

	std::ofstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	file << j.dump(2);
	return true;
}

std::optional<CacheManager::CacheMetadata> CacheManager::LoadMetadata(const std::string& filename)
{
	if (!fs::exists(filename))
	{
		return std::nullopt;
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		return std::nullopt;
	}

	try
	{
		nlohmann::json j;
		file >> j;

		CacheMetadata meta;
		meta.meshFile = j["mesh_file"];
		meta.configFile = j["config_file"];
		meta.combinedHash = j["combined_hash"];
		meta.meshHash = j["mesh_hash"];
		meta.configHash = j["config_hash"];
		meta.meshFileSize = j["mesh_file_size"];
		meta.configFileSize = j["config_file_size"];
		meta.cacheCreatedTime = j["cache_created_time"];
		meta.matrixHRows = j["matrix_h_rows"];
		meta.matrixHCols = j["matrix_h_cols"];
		meta.matrixHNonzeros = j["matrix_h_nonzeros"];
		meta.matrixCRows = j["matrix_c_rows"];
		meta.matrixCCols = j["matrix_c_cols"];
		meta.matrixCNonzeros = j["matrix_c_nonzeros"];
		meta.vectorPSize = j["vector_p_size"];
		meta.hasCapacityMatrix = j["has_capacity_matrix"];

		return meta;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to parse metadata: {}", e.what());
		return std::nullopt;
	}
}

bool CacheManager::ValidateInputFiles(
	const std::string& meshFile,
	const std::string& configFile,
	const CacheMetadata& meta)
{
	if (!fs::exists(meshFile))
	{
		LOG_ERROR("Mesh file not found: {}", meshFile);
		return false;
	}

	if (!fs::exists(configFile))
	{
		LOG_ERROR("Config file not found: {}", configFile);
		return false;
	}

	auto currentCombinedHash = HashUtils::ComputeMultiFileHash({ meshFile, configFile });

	if (!currentCombinedHash)
	{
		LOG_ERROR("Failed to compute current file hashes");
		return false;
	}

	if (*currentCombinedHash != meta.combinedHash)
	{
		LOG_WARN("Input files have changed!");
		LOG_WARN("  Expected hash: {}", meta.combinedHash);
		LOG_WARN("  Current hash:  {}", *currentCombinedHash);
		return false;
	}

	return true;
}

std::string CacheManager::GetCurrentTimestamp()
{
	using namespace std::chrono;

	const auto now = floor<seconds>(system_clock::now());
	const auto zoned = zoned_time{ current_zone(), now };

	return std::format("{:%Y-%m-%d %H:%M:%S}", zoned);
}

} // namespace fem::cache
