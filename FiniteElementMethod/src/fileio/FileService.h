#pragma once

#include "FileServiceError.h"

#include <filesystem>
#include <expected>
#include <string>

namespace fem::fileio
{

namespace fs = std::filesystem;

class FileService
{
public:
	FileService() = default;

	std::expected<std::string, FileServiceError> Read(const fs::path& path);
	std::expected<void, FileServiceError> Write(const fs::path& path, const std::string& content);
};

}
