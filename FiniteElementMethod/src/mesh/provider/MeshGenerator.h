#pragma once

#include "MeshLoader.h"
#include "MeshProviderError.h"
#include "mesh/model/Mesh.h"

#include <expected>
#include <filesystem>

namespace fem::mesh::provider
{

namespace fs = std::filesystem;

class MeshGenerator
{
public:
	std::expected<model::Mesh, MeshProviderError> GenerateFromGeo(const fs::path& path) const;

private:
	MeshLoader m_Loader;

	std::expected<fs::path, MeshProviderError> GenerateMshWithGmsh(const fs::path& path) const;
};

}



