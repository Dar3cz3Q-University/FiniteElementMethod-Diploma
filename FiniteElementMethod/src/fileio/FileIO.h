#pragma once

#include <filesystem>
#include <expected>
#include <memory>
#include <string>

#include "FileIOError.h"

namespace fem::fileio {

	class FileIO
	{
	public:
		FileIO();

		std::expected<std::string, FileIOError> read(const std::filesystem::path& path);
		void write(const std::filesystem::path& path, const std::string& content);
	};

}
