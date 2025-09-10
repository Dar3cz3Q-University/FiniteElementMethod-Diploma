#include "FileIO.h"

#include <filesystem>
#include <fstream>

namespace fem::fileio {

FileIO::FileIO()
{

}

std::expected<std::string, FileIOError> FileIO::read(const std::filesystem::path& path)
{
	namespace fs = std::filesystem;

	std::error_code ec;

	if (!fs::exists(path, ec))
	{
		return std::unexpected(FileIOError{ FileIOErrorCode::FileNotFound, path, ec.message() });
	}

	if (!fs::is_regular_file(path, ec))
	{
		return std::unexpected(FileIOError{ FileIOErrorCode::NotAFile, path, ec.message() });
	}

	if (ec)
	{
		return std::unexpected(FileIOError{ FileIOErrorCode::ReadFailure, path, ec.message() });
	}

	std::ifstream file(path, std::ios::in);
	if (!file)
	{
		return std::unexpected(FileIOError{ FileIOErrorCode::PermissionDenied, path, "Failed to open file." });
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();

	if (!file.good() && !file.eof())
	{
		return std::unexpected(FileIOError{ FileIOErrorCode::ReadFailure, path, "Failed to read file." });
	}

	return buffer.str();
}

void FileIO::write(const std::filesystem::path& path, const std::string& content)
{

}

}
