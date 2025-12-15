#include "StatsExporter.h"

#include "FileService.h"

#include <format>

namespace fem::fileio
{

std::expected<void, std::string> StatsExporter::Export(const fs::path& path, const std::string_view& solverName, const solver::FEMSolverStats& stats)
{
	LOG_INFO("Exporting metrics to {} file", path.string());

	std::string out;

	out += exportHeader;

	out += std::format(
		"{},{},{},{},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.3f},{:.2e}\n",
		solverName,
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
			std::format("Failed to export steady stats: {}", wr.error().message)
		);

	return {};
}

}


