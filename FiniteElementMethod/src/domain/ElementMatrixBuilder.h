#pragma once

#include "BoundaryMatrices.h"
#include "BoundaryCondition.h"
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
	ElementMatrixBuilder(const Material& material, const BoundaryCondition& bc) : m_Material(material), m_BoundaryCondition(bc)
	{
		LOG_INFO("Initialized with material = {}", material.name);
		LOG_INFO("Initialized with boundary condition group = {}", bc.physicalGroupName);
	}

	// TODO: Create custom error
	std::expected<ElementMatrices, int> BuildQuadMatrices(
		const mesh::model::Mesh& mesh,
		const mesh::model::Quad& quad) const;

	std::expected<BoundaryMatrices, int> BuildLineBoundaryMatrices(
		const mesh::model::Mesh& mesh,
		const mesh::model::Line& line) const;

private:
	const Material m_Material;
	const BoundaryCondition m_BoundaryCondition;
};

}
