#pragma once

#include "MeshProviderError.h"

#include "mesh/model/model.h"

#include <expected>
#include <filesystem>

namespace fem::mesh::provider
{

namespace fs = std::filesystem;

class MeshLoader
{
public:
	std::expected<model::Mesh, MeshProviderError> Load(const fs::path& path) const;
};

inline static bool isLine(int t);
inline static bool isQuad(int t);

}
