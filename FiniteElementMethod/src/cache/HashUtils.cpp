#include "HashUtils.h"

#include "logger/logger.h"

#include <fstream>
#include <filesystem>
#include <xxhash.h>
#include <format>

// TODO: Use std::expected and fileio module
namespace fem::cache
{

namespace fs = std::filesystem;

std::optional<std::string> HashUtils::ComputeFileHash(const std::string& filename)
{
	if (!fs::exists(filename))
	{
		LOG_ERROR("File not found for hashing: {}", filename);
		return std::nullopt;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		LOG_ERROR("Cannot open file for hashing: {}", filename);
		return std::nullopt;
	}

	XXH64_state_t* state = XXH64_createState();
	XXH64_reset(state, 0);

	constexpr size_t bufferSize = 8192;
	std::vector<char> buffer(bufferSize);

	while (file.read(buffer.data(), bufferSize) || file.gcount() > 0)
	{
		XXH64_update(state, buffer.data(), file.gcount());
	}

	uint64_t hash = XXH64_digest(state);
	XXH64_freeState(state);

	return std::format("{:016x}", hash);
}

std::optional<std::string> HashUtils::ComputeMultiFileHash(const std::vector<std::string>& filenames)
{
	XXH64_state_t* state = XXH64_createState();
	XXH64_reset(state, 0);

	std::vector<std::string> sortedFiles = filenames;
	std::sort(sortedFiles.begin(), sortedFiles.end());

	for (const auto& filename : sortedFiles)
	{
		if (!fs::exists(filename))
		{
			LOG_ERROR("File not found for multi-hash: {}", filename);
			XXH64_freeState(state);
			return std::nullopt;
		}

		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open())
		{
			LOG_ERROR("Cannot open file for multi-hash: {}", filename);
			XXH64_freeState(state);
			return std::nullopt;
		}

		constexpr size_t bufferSize = 8192;
		std::vector<char> buffer(bufferSize);

		while (file.read(buffer.data(), bufferSize) || file.gcount() > 0)
		{
			XXH64_update(state, buffer.data(), file.gcount());
		}
	}

	uint64_t hash = XXH64_digest(state);
	XXH64_freeState(state);

	return std::format("{:016x}", hash);
}

std::string HashUtils::ComputeStringHash(const std::string& content)
{
	uint64_t hash = XXH64(content.data(), content.size(), 0);
	return std::format("{:016x}", hash);
}

std::optional<std::string> HashUtils::ValidateAndHash(const std::string& filename)
{
	if (!fs::exists(filename))
	{
		return std::nullopt;
	}
	return ComputeFileHash(filename);
}

} // namespace fem::cache
