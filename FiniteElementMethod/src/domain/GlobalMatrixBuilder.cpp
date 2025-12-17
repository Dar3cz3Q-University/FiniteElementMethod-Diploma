#include "GlobalMatrixBuilder.h"

#include "logger/logger.h"

#include <atomic>
#include <iostream>
#include <vector>
#include <omp.h>

namespace fem::domain
{

std::expected<GlobalMatrices, int> fem::domain::GlobalMatrixBuilder::Build() const
{
	LOG_INFO("Calculating H, C matrices and P vector");

	const auto numberOfNodes = m_Mesh.GetNodesCount();
	const auto& quads = m_Mesh.GetQuads();
	const auto numberOfElements = quads.size();
	const auto& lines = m_Mesh.GetLines();
	const auto numberOfLines = lines.size();

	TripletsVector tripletsH;
	TripletsVector tripletsC;
	Vec globalP = Vec::Zero(numberOfNodes);

	tripletsH.reserve(numberOfElements * 16); // TODO: Move number to variable
	tripletsC.reserve(numberOfElements * 16);

	std::atomic<int> processedElements{ 0 };
	std::atomic<int> lastLoggedPercent{ 0 };
	const int totalElements = static_cast<int>(numberOfElements);

#pragma omp parallel
	{
		TripletsVector localTripletsH;
		TripletsVector localTripletsC;
		Vec localP = Vec::Zero(numberOfNodes);

#pragma omp for nowait
		for (int i = 0; i < numberOfElements; i++)
		{
			const auto& element = quads.at(i);
			auto res = m_Builder.BuildQuadMatrices(m_Mesh, element);
			if (!res) continue; // TODO: Handle error

			AssembleQuadElement(element, *res, localTripletsH, localTripletsC, localP);

			int done = ++processedElements;
			int currentPercent = (done * 100) / totalElements;
			int roundedPercent = (currentPercent / 10) * 10;

			if (roundedPercent > lastLoggedPercent.load(std::memory_order_relaxed))
			{
				int expected = roundedPercent - 10;
				if (lastLoggedPercent.compare_exchange_strong(expected, roundedPercent))
				{
					LOG_INFO("Assembling elements... {}% ({}/{})", roundedPercent, done, totalElements);
				}
			}
		}

#pragma omp critical
		{
			tripletsH.insert(tripletsH.end(), localTripletsH.begin(), localTripletsH.end());
			tripletsC.insert(tripletsC.end(), localTripletsC.begin(), localTripletsC.end());
			globalP += localP;
		}
	}

	// TODO: Check how boundary conditions are handled
	// TODO: Display progress for lines
	for (auto& line : lines)
	{
		const auto& res = m_Builder.BuildLineBoundaryMatrices(m_Mesh, line);
		if (!res) continue;

		AssembleLineElement(line, *res, tripletsH, globalP);
	}

	GlobalMatrices out;
	out.H.resize(numberOfNodes, numberOfNodes);
	out.C.resize(numberOfNodes, numberOfNodes);
	out.P = globalP;

	out.H.setFromTriplets(tripletsH.begin(), tripletsH.end());
	out.C.setFromTriplets(tripletsC.begin(), tripletsC.end());

	LOG_INFO("Matrix H statistics:");
	LOG_INFO("  Size: {} x {}", out.H.rows(), out.H.cols());
	LOG_INFO("  Non-zeros: {}", out.H.nonZeros());
	LOG_INFO("  Fill ratio: {:.4f}%", 100.0 * out.H.nonZeros() / (out.H.rows() * out.H.cols()));

	return out;
}

void GlobalMatrixBuilder::AssembleQuadElement(const mesh::model::Quad& element, const ElementMatrices& res, TripletsVector& localTripletsH, TripletsVector& localTripletsC, Vec& localP) const
{
	std::array<std::size_t, 4> nodeIds{};
	for (int localNode = 0; localNode < 4; ++localNode)
	{
		const auto gmshId = element.nodeIDs[localNode];
		nodeIds[localNode] = m_Mesh.GetNodeLocalId(gmshId);
	}

	for (int iLocal = 0; iLocal < 4; ++iLocal)
	{
		const auto globalI = nodeIds[iLocal];

		for (int jLocal = 0; jLocal < 4; ++jLocal)
		{
			const auto globalJ = nodeIds[jLocal];

			localTripletsH.emplace_back(globalI, globalJ, res.H(iLocal, jLocal));
			localTripletsC.emplace_back(globalI, globalJ, res.C(iLocal, jLocal));
		}

		localP(globalI) += res.P(iLocal);
	}
}

void GlobalMatrixBuilder::AssembleLineElement(const mesh::model::Line& element, const BoundaryMatrices& res, TripletsVector& localTripletsH, Vec& localP) const
{
	std::array<std::size_t, 2> nodeIds{};
	for (int localNode = 0; localNode < 2; ++localNode)
	{
		const auto gmshId = element.nodeIDs[localNode];
		nodeIds[localNode] = m_Mesh.GetNodeLocalId(gmshId);
	}

	for (int iLocal = 0; iLocal < 2; ++iLocal)
	{
		const auto globalI = nodeIds[iLocal];

		for (int jLocal = 0; jLocal < 2; ++jLocal)
		{
			const auto globalJ = nodeIds[jLocal];

			localTripletsH.emplace_back(globalI, globalJ, res.H(iLocal, jLocal));
		}

		localP(globalI) += res.P(iLocal);
	}
}

}
