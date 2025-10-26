#pragma once

#include <filesystem>
#include <string>

namespace fem::fileio {

enum class FileIOErrorCode
{
	FileNotFound,
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

	friend std::ostream& operator<<(std::ostream& os, FileIOError err)
	{
		os << "FileIOError[" << err.path << "]: ";
		switch (err.code)
		{
		case FileIOErrorCode::FileNotFound: os << "File not found."; break;
		case FileIOErrorCode::PermissionDenied: os << "Permission denied."; break;
		case FileIOErrorCode::NotAFile: os << "Path is not a regular file."; break;
		case FileIOErrorCode::ReadFailure: os << "Failed to read file."; break;
		case FileIOErrorCode::Unknown: os << "Unknown error."; break;
		}
		if (!err.message.empty()) os << " (" << err.message << ")";
		return os;
	}
};

}
