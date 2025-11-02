#pragma once

#include <filesystem>
#include <string>

namespace fem::fileio {

enum class FileIOErrorCode
{
	FileNotFound = 0,
	ParentDirectoryNotFound,
	PermissionDenied,
	NotAFile,
	ReadFailure,
	WriteFailure,
	Unknown
};

struct FileIOError
{
	FileIOErrorCode code;
	std::filesystem::path path;
	std::string message;
};

inline std::string ErrorToString(const FileIOError& err)
{
	using enum FileIOErrorCode;

	std::string msg = std::format("FileIOError[{}]: ", err.path.string());

	switch (err.code)
	{
	case FileNotFound: msg += "File not found.\n"; break;
	case ParentDirectoryNotFound: msg += "Parent directory not found.\n"; break;
	case PermissionDenied: msg += "Permission denied.\n"; break;
	case NotAFile: msg += "Path is not a regular file.\n"; break;
	case ReadFailure: msg += "Failed to read file.\n"; break;
	case WriteFailure: msg += "Failed to write file.\n"; break;
	case Unknown: msg += "Unknown error.\n"; break;
	}

	if (!err.message.empty())
	{
		msg += "(";
		msg += err.message;
		msg += ")";
	}

	return msg;
}

}

template <>
struct std::formatter<fem::fileio::FileIOError> : std::formatter<std::string_view>
{
	auto format(const fem::fileio::FileIOError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(fem::fileio::ErrorToString(err), ctx);
	}
};
