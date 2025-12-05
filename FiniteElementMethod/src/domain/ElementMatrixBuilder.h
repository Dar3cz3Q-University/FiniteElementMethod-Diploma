#pragma once

#include "BoundaryMatrices.h"
#include "model/BoundaryCondition.h"
#include "ElementMatrices.h"
#include "model/Material.h"

#include "logger/logger.h"
#include "mesh/mesh.h"

#include <expected>

namespace fem::domain
{

class ElementMatrixBuilder
{
public:
	ElementMatrixBuilder(const model::Material& material, const model::BoundaryCondition& bc) : m_Material(material), m_BoundaryCondition(bc)
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
	const model::Material m_Material;
	const model::BoundaryCondition m_BoundaryCondition;
};

}
