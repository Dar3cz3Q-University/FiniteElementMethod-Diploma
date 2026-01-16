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

	stats.elementCount = numberOfElements;
	stats.boundaryElementCount = numberOfLines;

	TripletsVector tripletsH;
	TripletsVector tripletsC;
	Vec globalP = Vec::Zero(numberOfNodes);

	tripletsH.reserve(numberOfElements * 16);
	tripletsC.reserve(numberOfElements * 16);

	std::atomic<int> processedElements{ 0 };
	std::atomic<int> lastLoggedPercent{ 0 };
	std::atomic<bool> hasError{ false };
	const int totalElements = static_cast<int>(numberOfElements);

	// Per-thread triplet statistics
	int maxThreads = omp_get_max_threads();
	std::vector<size_t> perThreadTripletsH(maxThreads, 0);
	std::vector<size_t> perThreadTripletsC(maxThreads, 0);
	std::vector<size_t> perThreadCapacityH(maxThreads, 0);
	std::vector<size_t> perThreadCapacityC(maxThreads, 0);

	auto elementStart = Now();

#pragma omp parallel
	{
		const int threadId = omp_get_thread_num();
		const int numThreads = omp_get_num_threads();
		const size_t estimatedPerThread = (numberOfElements / numThreads + 1) * 16;

		TripletsVector localTripletsH;
		TripletsVector localTripletsC;
		localTripletsH.reserve(estimatedPerThread);
		localTripletsC.reserve(estimatedPerThread);

		Vec localP = Vec::Zero(numberOfNodes);

#pragma omp for schedule(dynamic, 128)
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

		// Store per-thread statistics before merge
		perThreadTripletsH[threadId] = localTripletsH.size();
		perThreadTripletsC[threadId] = localTripletsC.size();
		perThreadCapacityH[threadId] = localTripletsH.capacity();
		perThreadCapacityC[threadId] = localTripletsC.capacity();

		auto mergeStart = Now();

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

#pragma omp single
		{
			stats.mergeTimeMs = ElapsedMs(mergeStart, Now());
		}
	}

	if (hasError.load())
	{
		LOG_ERROR("Element assembly failed");
		return std::unexpected(-1);
	}

	auto elementEnd = Now();
	stats.elementAssemblyTimeMs = ElapsedMs(elementStart, elementEnd) - stats.mergeTimeMs;

	auto boundaryStart = Now();

	int processedLines{};
	int lastLoggedLinePercent{};
	const int totalLines = static_cast<int>(numberOfLines);

	for (size_t i = 0; i < numberOfLines; i++)
	{
		const auto& line = lines[i];
		const auto& res = m_Builder.BuildLineBoundaryMatrices(m_Mesh, line);
		if (!res)
		{
			hasError.store(true, std::memory_order_relaxed);
			LOG_ERROR("Failed to build matrices for boundary line {}", i);
			continue;
		}

		AssembleLineElement(line, *res, tripletsH, globalP);

		int done = ++processedLines;
		int currentPercent = (done * 100) / totalLines;
		int roundedPercent = (currentPercent / 10) * 10;

		if (roundedPercent > lastLoggedLinePercent)
		{
			int expected = roundedPercent - 10;
			if (roundedPercent > lastLoggedLinePercent)
			{
				lastLoggedLinePercent = roundedPercent;
				LOG_INFO("Assembling boundary... {}% ({}/{})", roundedPercent, done, totalLines);
			}
		}
	}

	if (hasError.load())
	{
		LOG_ERROR("Boundary line assembly failed");
		return std::unexpected(-1);
	}

	auto boundaryEnd = Now();
	stats.boundaryAssemblyTimeMs = ElapsedMs(boundaryStart, boundaryEnd);

	stats.tripletsHCount = tripletsH.size();
	stats.tripletsCCount = tripletsC.size();
	stats.tripletsMemoryBytes = (tripletsH.capacity() + tripletsC.capacity()) * sizeof(Triplet);

	GlobalMatrices out;
	out.H.resize(numberOfNodes, numberOfNodes);
	out.C.resize(numberOfNodes, numberOfNodes);
	out.P = globalP;

	auto tripletStart = Now();

	out.H.setFromTriplets(tripletsH.begin(), tripletsH.end());
	out.C.setFromTriplets(tripletsC.begin(), tripletsC.end());

	auto tripletEnd = Now();
	stats.tripletToSparseTimeMs = ElapsedMs(tripletStart, tripletEnd);

	size_t nnzH = out.H.nonZeros();
	size_t nnzC = out.C.nonZeros();
	stats.sparseMatrixMemoryBytes =
		(nnzH + nnzC) * (sizeof(double) + sizeof(int)) +
		2 * (numberOfNodes + 1) * sizeof(int) +
		numberOfNodes * sizeof(double);

	auto totalEnd = Now();
	stats.totalAssemblyTimeMs = ElapsedMs(totalStart, totalEnd);

	// Memory statistics before factorization
	LOG_INFO("=== Memory usage before factorization ===");
	LOG_INFO("Triplet size: sizeof(Triplet) = {} bytes", sizeof(Triplet));

	LOG_INFO("Per-thread triplet counts:");
	size_t totalThreadLocalTripletsH = 0;
	size_t totalThreadLocalTripletsC = 0;
	size_t totalThreadLocalCapacityH = 0;
	size_t totalThreadLocalCapacityC = 0;
	for (int t = 0; t < maxThreads; ++t)
	{
		if (perThreadTripletsH[t] > 0 || perThreadTripletsC[t] > 0)
		{
			LOG_INFO("  Thread {}: H = {} triplets (capacity: {}), C = {} triplets (capacity: {})",
				t, perThreadTripletsH[t], perThreadCapacityH[t], perThreadTripletsC[t], perThreadCapacityC[t]);
			totalThreadLocalTripletsH += perThreadTripletsH[t];
			totalThreadLocalTripletsC += perThreadTripletsC[t];
			totalThreadLocalCapacityH += perThreadCapacityH[t];
			totalThreadLocalCapacityC += perThreadCapacityC[t];
		}
	}

	size_t totalThreadLocalBytes = (totalThreadLocalCapacityH + totalThreadLocalCapacityC) * sizeof(Triplet);
	double totalThreadLocalMB = static_cast<double>(totalThreadLocalBytes) / (1024.0 * 1024.0);
	LOG_INFO("Total thread-local triplets: H = {}, C = {}", totalThreadLocalTripletsH, totalThreadLocalTripletsC);
	LOG_INFO("Total thread-local lists size: {:.2f} MB (capacity: {} H + {} C triplets)",
		totalThreadLocalMB, totalThreadLocalCapacityH, totalThreadLocalCapacityC);

	LOG_INFO("Global matrix after aggregation:");
	LOG_INFO("  Matrix H: nnz = {}, size = {:.2f} MB",
		nnzH, static_cast<double>(nnzH * (sizeof(double) + sizeof(int)) + (numberOfNodes + 1) * sizeof(int)) / (1024.0 * 1024.0));
	LOG_INFO("  Matrix C: nnz = {}, size = {:.2f} MB",
		nnzC, static_cast<double>(nnzC * (sizeof(double) + sizeof(int)) + (numberOfNodes + 1) * sizeof(int)) / (1024.0 * 1024.0));
	LOG_INFO("  Vector P: size = {:.2f} MB", static_cast<double>(numberOfNodes * sizeof(double)) / (1024.0 * 1024.0));
	LOG_INFO("==========================================");

	LOG_INFO("Matrix H statistics:");
	LOG_INFO("  Size: {} x {}", out.H.rows(), out.H.cols());
	LOG_INFO("  Non-zeros: {} (H), {} (C)", nnzH, nnzC);
	LOG_INFO("  Fill ratio: {:.4f}%", 100.0 * nnzH / (out.H.rows() * out.H.cols()));

	LOG_INFO("Assembly timing:");
	LOG_INFO("  Element assembly: {:.2f} ms ({:.0f} elem/s)", stats.elementAssemblyTimeMs, stats.getElementsPerSecond());
	LOG_INFO("  Boundary assembly: {:.2f} ms ({:.0f} elem/s)", stats.boundaryAssemblyTimeMs, stats.getBoundaryElementsPerSecond());
	LOG_INFO("  Merge time: {:.2f} ms", stats.mergeTimeMs);
	LOG_INFO("  Triplet to sparse: {:.2f} ms", stats.tripletToSparseTimeMs);
	LOG_INFO("  Total: {:.2f} ms", stats.totalAssemblyTimeMs);

	LOG_INFO("Memory usage:");
	LOG_INFO("  Triplets: {:.2f} MB ({} H, {} C)", stats.getTripletsMemoryMB(), stats.tripletsHCount, stats.tripletsCCount);
	LOG_INFO("  Sparse matrices: {:.2f} MB", stats.getSparseMatrixMemoryMB());

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
