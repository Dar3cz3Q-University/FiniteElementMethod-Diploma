#pragma once

#include "MeshLoader.h"
#include "MeshGenerator.h"
#include "MeshProviderError.h"
#include "mesh/model/Mesh.h"

#include <expected>
#include <filesystem>

namespace fem::mesh::provider
{

namespace fs = std::filesystem;

class MeshProvider
{
public:
	MeshProvider() = default;

	std::expected<model::Mesh, MeshProviderError> LoadMesh(const fs::path& path) const;

private:
	MeshLoader m_Loader;
	MeshGenerator m_Generator;
};

}
