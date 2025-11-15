#include "GlobalMatrixBuilder.h"

#include "logger/logger.h"

#include <iostream>

namespace fem::domain
{

std::expected<GlobalMatrices, int> fem::domain::GlobalMatrixBuilder::Build() const
{
	LOG_INFO("Calculating H, C matrices and P vector");

	const auto& numberOfNodes = m_Mesh.GetNodesCount();

	GlobalMatrices out;
	out.H.setZero(numberOfNodes, numberOfNodes);
	out.C.setZero(numberOfNodes, numberOfNodes);
	out.P.setZero(numberOfNodes, 1);

	// Calculate H, C, P for each Quad
	for (auto& element : m_Mesh.GetQuads())
	{
		const auto& res = m_Builder.BuildQuadMatrices(m_Mesh, element);
		if (!res) continue;

		std::cout << res->H << "\n";
		std::cout << "---" << "\n";
		std::cout << res->C << "\n";
		std::cout << "-----" << "\n";
	}

	// Calculate H, C, P for each Line

	// Aggregate matrices

	return out;
}

}
