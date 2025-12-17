#pragma once

#include "AssemblyStats.h"
#include "GlobalMatrices.h"
#include "ElementMatrixBuilder.h"

#include "mesh/mesh.h"

namespace fem::domain
{

using TripletsVector = std::vector<Triplet>;

struct GlobalMatrixBuildResult
{
	GlobalMatrices matrices;
	AssemblyStats stats;
};

class GlobalMatrixBuilder
{
public:
	GlobalMatrixBuilder(const mesh::model::Mesh& mesh, const ElementMatrixBuilder& builder) : m_Mesh(mesh), m_Builder(builder) {};

	// TODO: Create custom error
	std::expected<GlobalMatrixBuildResult, int> Build() const;

private:
	void AssembleQuadElement(const mesh::model::Quad& element, const ElementMatrices& res, TripletsVector& localTripletsH, TripletsVector& localTripletsC, Vec& localP) const;
	void AssembleLineElement(const mesh::model::Line& element, const BoundaryMatrices& res, TripletsVector& localTripletsH, Vec& localP) const;

private:
	const mesh::model::Mesh& m_Mesh;
	const ElementMatrixBuilder& m_Builder;
};

}
