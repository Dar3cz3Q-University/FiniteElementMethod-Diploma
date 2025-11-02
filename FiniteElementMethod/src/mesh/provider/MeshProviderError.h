#pragma once

#include <string>
#include <format>

namespace fem::mesh::provider
{

enum class MeshProviderErrorCode
{
	Unknown
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
struct std::formatter<fem::mesh::provider::MeshProviderError> : std::formatter<std::string_view>
{
	auto format(const fem::mesh::provider::MeshProviderError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(fem::mesh::provider::ErrorToString(err), ctx);
	}
};
