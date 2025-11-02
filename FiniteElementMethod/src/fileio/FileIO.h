#pragma once

#include <filesystem>
#include <expected>
#include <memory>
#include <string>

#include "FileIOError.h"

namespace fem::fileio
{

namespace fs = std::filesystem;

class FileIO
{
public:
	FileIO() = default;

	std::expected<std::string, FileIOError> Read(const fs::path& path);
	std::expected<void, FileIOError> Write(const fs::path& path, const std::string& content);
};

}
