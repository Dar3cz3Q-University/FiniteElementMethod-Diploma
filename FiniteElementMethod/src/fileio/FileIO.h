#pragma once

#include <filesystem>
#include <expected>
#include <memory>
#include <string>

#include "FileIOError.h"

namespace fem::fileio
{

class FileIO
{
public:
	FileIO() = default;

	std::expected<std::string, FileIOError> Read(const std::filesystem::path& path);
	std::expected<void, FileIOError> Write(const std::filesystem::path& path, const std::string& content);
};

}
