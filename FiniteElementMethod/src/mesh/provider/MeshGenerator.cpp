#include "MeshGenerator.h"

#include "gmsh.h"

namespace fem::mesh::provider
{

std::expected<model::Mesh, MeshProviderError> MeshGenerator::GenerateFromGeo(const fs::path& path) const
{
	auto mshPath = GenerateMshWithGmsh(path);

	if (!mshPath)
		return std::unexpected(mshPath.error());

	return m_Loader.Load(*mshPath);
}

std::expected<fs::path, MeshProviderError> MeshGenerator::GenerateMshWithGmsh(const fs::path& path) const
{
	if (!fs::exists(path))
	{
		// TODO: Return error
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "" });
	}

	fs::path resultPath = path;
	resultPath.replace_extension(".msh");

	try
	{
		gmsh::open(path.string());
		gmsh::model::mesh::generate(2);
		gmsh::write(resultPath.string());
	}
	catch (...)
	{
		// TODO: Return error
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "" });
	}

	// TODO: Return error
	if (!fs::exists(path))
	{
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "" });
	}

	return resultPath;
}

}
