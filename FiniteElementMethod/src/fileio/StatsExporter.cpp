#include "StatsExporter.h"

#include "FileService.h"
#include "config/CompileConfig.h"
#include "logger/logger.h"

#include <format>
#include <nlohmann/json.hpp>

namespace fem::fileio
{

std::expected<void, std::string> StatsExporter::Export(const fs::path& path, const FullMetrics& metrics)
{
	if (path.extension() == ".json")
	{
		return ExportJSON(path, metrics);
	}

	return ExportCSV(path, metrics);
}

std::expected<void, std::string> StatsExporter::ExportCSV(const fs::path& path, const FullMetrics& metrics)
{
	LOG_INFO("Exporting metrics to {} (CSV)", path.string());

	constexpr std::string_view exportHeader =
		"Solver,StorageFormat,Reordering,ProblemSize,NonZeros,"
		"FactorizationMs,SolveMs,TotalSolverMs,TotalMs,OverheadMs,"
		"MemoryUsedMB,PeakMemoryMB,ResidualNorm\n";

	const auto& stats = metrics.solverStats;

	std::string out;

	out += exportHeader;

	out += std::format(
		"{},{},{},{},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.2e}\n",
		metrics.solverName,
		fem::config::StorageOrderName,
		fem::config::ReorderingName,
		stats.matrixSize,
		stats.matrixNonZeros,
		stats.getAvgFactorizationMs(),
		stats.getAvgSolveMs(),
		stats.getAvgPerStepMs(),
		stats.totalTimeMs,
		stats.overheadMs,
		stats.getMemoryUsedMB(),
		stats.getPeakMemoryMB(),
		stats.residualNorm
	);

	FileService writer;
	auto wr = writer.Write(path, out);

	if (!wr)
		return std::unexpected(
			std::format("Failed to export metrics: {}", wr.error().message)
		);

	return {};
}

std::expected<void, std::string> StatsExporter::ExportJSON(const fs::path& path, const FullMetrics& metrics)
{
	LOG_INFO("Exporting metrics to {} file (JSON)", path.string());

	nlohmann::json json;

	// Solver info
	json["solver"]["name"] = metrics.solverName;
	json["solver"]["storageFormat"] = fem::config::StorageOrderName;
	json["solver"]["reordering"] = fem::config::ReorderingName;

	// Matrix info
	const auto& ss = metrics.solverStats;
	json["matrix"]["size"] = ss.matrixSize;
	json["matrix"]["nonZeros"] = ss.matrixNonZeros;

	// Solver timing
	json["timing"]["solver"]["factorizationMs"] = ss.getAvgFactorizationMs();
	json["timing"]["solver"]["solveMs"] = ss.getAvgSolveMs();
	json["timing"]["solver"]["totalSolverMs"] = ss.getAvgPerStepMs();
	json["timing"]["solver"]["setupMs"] = ss.setupTimeMs;
	json["timing"]["solver"]["overheadMs"] = ss.overheadMs;
	json["timing"]["solver"]["overheadPercent"] = ss.getOverheadPercent();
	json["timing"]["totalMs"] = ss.totalTimeMs;
	json["timing"]["linearSolveCount"] = ss.linearSolveCount;

	// Memory
	json["memory"]["solver"]["usedMB"] = ss.getMemoryUsedMB();
	json["memory"]["solver"]["peakMB"] = ss.getPeakMemoryMB();
	json["memory"]["solver"]["usedBytes"] = ss.memoryUsedBytes;
	json["memory"]["solver"]["peakBytes"] = ss.peakMemoryBytes;

	// Residuals
	json["residual"]["norm"] = ss.residualNorm;
	json["residual"]["min"] = ss.minResidual;
	json["residual"]["max"] = ss.maxResidual;

	// Assembly stats
	if (metrics.assemblyStats.has_value())
	{
		const auto& as = *metrics.assemblyStats;

		json["assembly"]["timing"]["elementMs"] = as.elementAssemblyTimeMs;
		json["assembly"]["timing"]["boundaryMs"] = as.boundaryAssemblyTimeMs;
		json["assembly"]["timing"]["mergeMs"] = as.mergeTimeMs;
		json["assembly"]["timing"]["tripletToSparseMs"] = as.tripletToSparseTimeMs;
		json["assembly"]["timing"]["totalMs"] = as.totalAssemblyTimeMs;
		json["assembly"]["timing"]["overheadMs"] = as.getOverheadMs();
		json["assembly"]["timing"]["overheadPercent"] = as.getOverheadPercent();

		json["assembly"]["counts"]["elements"] = as.elementCount;
		json["assembly"]["counts"]["boundaryElements"] = as.boundaryElementCount;
		json["assembly"]["counts"]["tripletsH"] = as.tripletsHCount;
		json["assembly"]["counts"]["tripletsC"] = as.tripletsCCount;

		json["assembly"]["performance"]["elementsPerSecond"] = as.getElementsPerSecond();
		json["assembly"]["performance"]["boundaryElementsPerSecond"] = as.getBoundaryElementsPerSecond();

		json["assembly"]["memory"]["tripletsMB"] = as.getTripletsMemoryMB();
		json["assembly"]["memory"]["sparseMatrixMB"] = as.getSparseMatrixMemoryMB();
		json["assembly"]["memory"]["tripletsBytes"] = as.tripletsMemoryBytes;
		json["assembly"]["memory"]["sparseMatrixBytes"] = as.sparseMatrixMemoryBytes;
	}

	FileService writer;
	auto wr = writer.Write(path, json.dump(2));

	if (!wr)
		return std::unexpected(
			std::format("Failed to export metrics: {}", wr.error().message)
		);

	return {};
}

}


