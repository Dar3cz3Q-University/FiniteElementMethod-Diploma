#pragma once

#include "ElementMatrices.h"
#include "Material.h"

#include "logger/logger.h"
#include "mesh/mesh.h"

#include <expected>

namespace fem::domain
{

class ElementMatrixBuilder
{
public:
	ElementMatrixBuilder(const Material& material) : m_Material(material)
	{
		LOG_INFO("Initialized with material = {}", material.name);
	}

	// TODO: Create custom error
	std::expected<ElementMatrices, int> BuildQuadMatrices(const mesh::model::Mesh& mesh, const mesh::model::Quad& quad) const;

private:
	const Material m_Material;
};

}
