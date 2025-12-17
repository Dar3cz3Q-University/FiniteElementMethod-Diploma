#include "GlobalMatrixBuilder.h"

#include "logger/logger.h"
#include "utils/utils.h"

#include <atomic>
#include <iostream>
#include <vector>
#include <omp.h>

namespace fem::domain
{

std::expected<GlobalMatrixBuildResult, int> fem::domain::GlobalMatrixBuilder::Build() const
{
	LOG_INFO("Calculating H, C matrices and P vector");

	auto totalStart = Now();
	AssemblyStats stats;

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
	std::atomic<bool> hasError{ false };
	const int totalElements = static_cast<int>(numberOfElements);

	auto elementStart = Now();

#pragma omp parallel
	{
		TripletsVector localTripletsH;
		TripletsVector localTripletsC;
		Vec localP = Vec::Zero(numberOfNodes);

#pragma omp for schedule(dynamic, 64)
		for (int i = 0; i < totalElements; i++)
		{
			if (hasError.load(std::memory_order_relaxed)) continue;

			const auto& element = quads.at(i);
			auto res = m_Builder.BuildQuadMatrices(m_Mesh, element);

			if (!res)
			{
				hasError.store(true, std::memory_order_relaxed);
				LOG_ERROR("Failed to build matrices for element {}", i);
				continue;
			}

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

#pragma omp critical(merge_H)
		{
			tripletsH.insert(tripletsH.end(), localTripletsH.begin(), localTripletsH.end());
		}

#pragma omp critical(merge_C)
		{
			tripletsC.insert(tripletsC.end(), localTripletsC.begin(), localTripletsC.end());
		}

#pragma omp critical(merge_P)
		{
			globalP += localP;
		}
	}

	if (hasError.load())
	{
		LOG_ERROR("Element assembly failed");
		return std::unexpected(-1);
	}

	auto elementEnd = Now();
	stats.elementAssemblyTimeMs = ElapsedMs(elementStart, elementEnd);

	// TODO: Check how boundary conditions are handled
	// TODO: Display progress for lines
	auto boundaryStart = Now();

	for (auto& line : lines)
	{
		const auto& res = m_Builder.BuildLineBoundaryMatrices(m_Mesh, line);
		if (!res) continue;

		AssembleLineElement(line, *res, tripletsH, globalP);
	}

	auto boundaryEnd = Now();
	stats.boundaryAssemblyTimeMs = ElapsedMs(boundaryStart, boundaryEnd);

	GlobalMatrices out;
	out.H.resize(numberOfNodes, numberOfNodes);
	out.C.resize(numberOfNodes, numberOfNodes);
	out.P = globalP;

	auto tripletStart = Now();

	out.H.setFromTriplets(tripletsH.begin(), tripletsH.end());
	out.C.setFromTriplets(tripletsC.begin(), tripletsC.end());

	auto tripletEnd = Now();
	stats.tripletToSparseTimeMs = ElapsedMs(tripletStart, tripletEnd);

	auto totalEnd = Now();
	stats.totalAssemblyTimeMs = ElapsedMs(totalStart, totalEnd);

	LOG_INFO("Matrix H statistics:");
	LOG_INFO("  Size: {} x {}", out.H.rows(), out.H.cols());
	LOG_INFO("  Non-zeros: {}", out.H.nonZeros());
	LOG_INFO("  Fill ratio: {:.4f}%", 100.0 * out.H.nonZeros() / (out.H.rows() * out.H.cols()));

	LOG_INFO("Assembly timing:");
	LOG_INFO("  Element assembly: {:.2f} ms", stats.elementAssemblyTimeMs);
	LOG_INFO("  Boundary assembly: {:.2f} ms", stats.boundaryAssemblyTimeMs);
	LOG_INFO("  Triplet to sparse: {:.2f} ms", stats.tripletToSparseTimeMs);
	LOG_INFO("  Total: {:.2f} ms", stats.totalAssemblyTimeMs);

	return GlobalMatrixBuildResult{ .matrices = std::move(out), .stats = stats };
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
