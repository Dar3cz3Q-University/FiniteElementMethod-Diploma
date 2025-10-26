#include "FileIO.h"

#include <filesystem>
#include <fstream>

namespace fem::fileio {

std::expected<std::string, FileIOError> FileIO::Read(const std::filesystem::path& path)
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

std::expected<void, FileIOError> FileIO::Write(const std::filesystem::path& path, const std::string& content)
{
    namespace fs = std::filesystem;

    std::error_code ec;

    if (!fs::exists(path, ec))
    {
        const auto parent = path.parent_path();
        if (!parent.empty() && !fs::exists(parent, ec))
        {
            return std::unexpected(FileIOError{ FileIOErrorCode::ParentDirectoryNotFound, path, ec.message() });
        }
    }
    else
    {
        if (!fs::is_regular_file(path, ec))
        {
            return std::unexpected(FileIOError{ FileIOErrorCode::NotAFile, path, ec.message() });
        }
    }

    if (ec)
    {
        return std::unexpected(FileIOError{ FileIOErrorCode::ReadFailure, path, ec.message() });
    }

    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (!file)
    {
        return std::unexpected(FileIOError{ FileIOErrorCode::PermissionDenied, path, "Failed to open file." });
    }

    file << content;

    if (!file.good())
    {
        return std::unexpected(FileIOError{ FileIOErrorCode::WriteFailure, path, "Failed to write file." });
    }

    return {};
}

}
