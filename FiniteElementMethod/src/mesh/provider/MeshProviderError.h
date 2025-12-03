#pragma once

#include <string>
#include <format>

namespace fem::mesh::provider
{

enum class MeshProviderErrorCode : int
{
	ExtensionNotSupported = 0,
	InputPathNotFound,
	OutOfMemory,
	OutputWriteFailed,
	GmshOpenFailed,
	GmshGenerateFailed,
	GmshWriteFailed,
	IoError,
	ResultMissing,
	Unknown,
};

struct MeshProviderError
{
	MeshProviderErrorCode code;
	std::string message;

	std::string ToString() const
	{
		using enum MeshProviderErrorCode;

		std::string msg = "MeshProviderCode: ";

		switch (code)
		{
		case ExtensionNotSupported:
			msg += "Extension of the file is not currently supported.\n";
			break;
		case InputPathNotFound:
			msg += "Input file path not found.\n";
			break;
		case OutOfMemory:
			msg += "Out of memory while processing mesh.\n";
			break;
		case OutputWriteFailed:
			msg += "Failed to write output mesh file.\n";
			break;
		case GmshOpenFailed:
			msg += "Failed to open input geometry file in Gmsh.\n";
			break;
		case GmshGenerateFailed:
			msg += "Mesh generation in Gmsh failed.\n";
			break;
		case GmshWriteFailed:
			msg += "Failed to write mesh file using Gmsh.\n";
			break;
		case IoError:
			msg += "I/O error occurred while accessing file system.\n";
			break;
		case ResultMissing:
			msg += "Expected mesh output file is missing after generation.\n";
			break;
		case Unknown:
			msg += "Unknown error.\n";
			break;
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
struct std::formatter<fem::mesh::provider::MeshProviderError> : std::formatter<std::string_view>
{
	auto format(const fem::mesh::provider::MeshProviderError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(err.ToString(), ctx);
	}
};
