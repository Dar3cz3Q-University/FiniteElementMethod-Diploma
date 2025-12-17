#pragma once

#include <filesystem>
#include <format>
#include <string>

namespace fem::fileio
{

namespace fs = std::filesystem;

enum class VTKExportErrorCode
{
	InvalidData,
	DirectoryCreationFailed,
	FileWriteFailed
};

struct VTKExportError
{
	VTKExportErrorCode code;
	fs::path path;
	std::string message;

	std::string ToString() const
	{
		std::string prefix;
		switch (code)
		{
		case VTKExportErrorCode::InvalidData:
			prefix = "Invalid data";
			break;
		case VTKExportErrorCode::DirectoryCreationFailed:
			prefix = "Directory creation failed";
			break;
		case VTKExportErrorCode::FileWriteFailed:
			prefix = "File write failed";
			break;
		}

		return std::format("VTK Export Error [{}]: {} ({})", prefix, message, path.string());
	}
};

} // namespace fem::fileio
