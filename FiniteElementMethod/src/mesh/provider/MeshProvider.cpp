#include "MeshProvider.h"

#include <format>

namespace fem::mesh::provider
{

std::expected<model::Mesh, MeshProviderError> MeshProvider::LoadMesh(const fs::path& path) const
{
	const auto ext = path.extension().string();

	if (ext == ".msh")
	{
		return m_Loader.Load(path);
	}
	else if (ext == ".geo")
	{
		return m_Generator.GenerateFromGeo(path);
	}

	return std::unexpected(MeshProviderError{ MeshProviderErrorCode::ExtensionNotSupported, std::format("Provided extension: {}", ext)});
}

}
