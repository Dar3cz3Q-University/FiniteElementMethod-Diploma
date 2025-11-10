#include "FileService.h"

#include <fstream>

namespace fem::fileio {

std::expected<std::string, FileServiceError> FileService::Read(const std::filesystem::path& path)
{
	std::error_code ec;

	if (!fs::exists(path, ec))
	{
		return std::unexpected(FileServiceError{ FileServiceErrorCode::FileNotFound, path, ec.message() });
	}

	if (!fs::is_regular_file(path, ec))
	{
		return std::unexpected(FileServiceError{ FileServiceErrorCode::NotAFile, path, ec.message() });
	}

	if (ec)
	{
		return std::unexpected(FileServiceError{ FileServiceErrorCode::ReadFailure, path, ec.message() });
	}

	std::ifstream file(path, std::ios::in);
	if (!file)
	{
		return std::unexpected(FileServiceError{ FileServiceErrorCode::PermissionDenied, path, "Failed to open file." });
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();

	if (!file.good() && !file.eof())
	{
		return std::unexpected(FileServiceError{ FileServiceErrorCode::ReadFailure, path, "Failed to read file." });
	}

	return buffer.str();
}

std::expected<void, FileServiceError> FileService::Write(const std::filesystem::path& path, const std::string& content)
{
    std::error_code ec;

    if (!fs::exists(path, ec))
    {
        const auto parent = path.parent_path();
        if (!parent.empty() && !fs::exists(parent, ec))
        {
            return std::unexpected(FileServiceError{ FileServiceErrorCode::ParentDirectoryNotFound, path, ec.message() });
        }
    }
    else
    {
        if (!fs::is_regular_file(path, ec))
        {
            return std::unexpected(FileServiceError{ FileServiceErrorCode::NotAFile, path, ec.message() });
        }
    }

    if (ec)
    {
        return std::unexpected(FileServiceError{ FileServiceErrorCode::ReadFailure, path, ec.message() });
    }

    std::ofstream file(path, std::ios::out | std::ios::trunc);
    if (!file)
    {
        return std::unexpected(FileServiceError{ FileServiceErrorCode::PermissionDenied, path, "Failed to open file." });
    }

    file << content;

    if (!file.good())
    {
        return std::unexpected(FileServiceError{ FileServiceErrorCode::WriteFailure, path, "Failed to write file." });
    }

    return {};
}

}
