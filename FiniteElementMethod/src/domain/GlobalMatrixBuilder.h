#pragma once

#include "GlobalMatrices.h"
#include "ElementMatrixBuilder.h"

#include "mesh/mesh.h"

namespace fem::domain
{

class GlobalMatrixBuilder
{
public:
	GlobalMatrixBuilder(const mesh::model::Mesh& mesh, const ElementMatrixBuilder& builder) : m_Mesh(mesh), m_Builder(builder) {};

	// TODO: Create custom error
	std::expected<GlobalMatrices, int> Build() const;

private:
	const mesh::model::Mesh& m_Mesh;
	const ElementMatrixBuilder& m_Builder;
};

}
