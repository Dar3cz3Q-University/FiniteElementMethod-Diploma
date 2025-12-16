#pragma once

#include <string>
#include <optional>
#include <vector>

namespace fem::cache
{

class HashUtils
{
public:
	static std::optional<std::string> ComputeFileHash(const std::string& filename);

	static std::optional<std::string> ComputeMultiFileHash(const std::vector<std::string>& filenames);

	static std::string ComputeStringHash(const std::string& content);

	static std::optional<std::string> ValidateAndHash(const std::string& filename);

private:
	HashUtils() = delete;
};

} // namespace fem::cache
