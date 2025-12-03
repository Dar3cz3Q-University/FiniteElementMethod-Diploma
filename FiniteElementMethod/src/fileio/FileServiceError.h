#pragma once

#include <filesystem>
#include <string>

namespace fem::fileio
{

enum class FileServiceErrorCode : int
{
	FileNotFound = 0,
	ParentDirectoryNotFound,
	PermissionDenied,
	NotAFile,
	ReadFailure,
	WriteFailure,
	Unknown
};

struct FileServiceError
{
	FileServiceErrorCode code;
	std::filesystem::path path;
	std::string message;

	std::string ToString() const
	{
		using enum FileServiceErrorCode;

		std::string msg = std::format("FileIOError[{}]: ", path.string());

		switch (code)
		{
		case FileNotFound: msg += "File not found.\n"; break;
		case ParentDirectoryNotFound: msg += "Parent directory not found.\n"; break;
		case PermissionDenied: msg += "Permission denied.\n"; break;
		case NotAFile: msg += "Path is not a regular file.\n"; break;
		case ReadFailure: msg += "Failed to read file.\n"; break;
		case WriteFailure: msg += "Failed to write file.\n"; break;
		case Unknown: msg += "Unknown error.\n"; break;
		}

		if (!message.empty())
		{
			msg += "(";
			msg += message;
			msg += ")";
		}

		return msg;
	}
};

}

template <>
struct std::formatter<fem::fileio::FileServiceError> : std::formatter<std::string_view>
{
	auto format(const fem::fileio::FileServiceError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(err.ToString(), ctx);
	}
};
