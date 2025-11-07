#pragma once

#include <string>
#include <format>

namespace fem::mesh::provider
{

enum class MeshProviderErrorCode
{
	Unknown,
	InputPathNotFound,
	OutOfMemory,
	OutputWriteFailed,
	GmshOpenFailed,
	GmshGenerateFailed,
	GmshWriteFailed,
	IoError,
	ResultMissing,
};

struct MeshProviderError
{
	MeshProviderErrorCode code;
	std::string message;
};

inline std::string ErrorToString(const MeshProviderError& err)
{
	using enum MeshProviderErrorCode;

	std::string msg = "MeshProviderCode: ";

	switch (err.code)
	{
	case MeshProviderErrorCode::Unknown:
		msg += "Unknown error.\n";
		break;
	case MeshProviderErrorCode::InputPathNotFound:
		msg += "Input file path not found.\n";
		break;
	case MeshProviderErrorCode::OutOfMemory:
		msg += "Out of memory while processing mesh.\n";
		break;
	case MeshProviderErrorCode::OutputWriteFailed:
		msg += "Failed to write output mesh file.\n";
		break;
	case MeshProviderErrorCode::GmshOpenFailed:
		msg += "Failed to open input geometry file in Gmsh.\n";
		break;
	case MeshProviderErrorCode::GmshGenerateFailed:
		msg += "Mesh generation in Gmsh failed.\n";
		break;
	case MeshProviderErrorCode::GmshWriteFailed:
		msg += "Failed to write mesh file using Gmsh.\n";
		break;
	case MeshProviderErrorCode::IoError:
		msg += "I/O error occurred while accessing file system.\n";
		break;
	case MeshProviderErrorCode::ResultMissing:
		msg += "Expected mesh output file is missing after generation.\n";
		break;
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
struct std::formatter<fem::mesh::provider::MeshProviderError> : std::formatter<std::string_view>
{
	auto format(const fem::mesh::provider::MeshProviderError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(fem::mesh::provider::ErrorToString(err), ctx);
	}
};
