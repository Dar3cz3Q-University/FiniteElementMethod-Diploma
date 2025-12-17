#include "VTKExporter.h"

#include "logger/logger.h"

#include <iomanip>
#include <sstream>

namespace fem::fileio
{

std::expected<void, VTKExportError> VTKExporter::ExportSteady(
	const fs::path& outputPath,
	const mesh::model::Mesh& mesh,
	const Vec& temperature,
	const std::string& fieldName)
{
	LOG_INFO("Exporting steady-state solution to: {}", outputPath.string());

	if (static_cast<size_t>(temperature.size()) != mesh.GetNodesCount())
	{
		return std::unexpected(VTKExportError{
			VTKExportErrorCode::InvalidData,
			outputPath,
			std::format("Temperature vector size ({}) doesn't match mesh nodes count ({})",
				temperature.size(), mesh.GetNodesCount())
		});
	}

	std::string content = GenerateVTKContent(mesh, temperature, fieldName, 0.0);

	FileService fileService;
	auto result = fileService.Write(outputPath, content);
	if (!result)
	{
		return std::unexpected(MapFileServiceError(result.error()));
	}

	LOG_INFO("VTK export completed successfully");
	return {};
}

std::expected<void, VTKExportError> VTKExporter::ExportTransient(
	const fs::path& outputDir,
	const mesh::model::Mesh& mesh,
	const std::vector<Vec>& temperatures,
	const std::vector<double>& timeSteps,
	const std::string& baseName,
	const std::string& fieldName)
{
	LOG_INFO("Exporting transient solution ({} time steps) to: {}", temperatures.size(), outputDir.string());

	if (temperatures.empty())
	{
		return std::unexpected(VTKExportError{
			VTKExportErrorCode::InvalidData,
			outputDir,
			"No temperature data to export"
		});
	}

	if (temperatures.size() != timeSteps.size())
	{
		return std::unexpected(VTKExportError{
			VTKExportErrorCode::InvalidData,
			outputDir,
			std::format("Temperature frames ({}) doesn't match time steps count ({})",
				temperatures.size(), timeSteps.size())
		});
	}

	std::error_code ec;
	fs::create_directories(outputDir, ec);
	if (ec)
	{
		return std::unexpected(VTKExportError{
			VTKExportErrorCode::DirectoryCreationFailed,
			outputDir,
			ec.message()
		});
	}

	FileService fileService;

	for (size_t i = 0; i < temperatures.size(); i++)
	{
		if (static_cast<size_t>(temperatures[i].size()) != mesh.GetNodesCount())
		{
			return std::unexpected(VTKExportError{
				VTKExportErrorCode::InvalidData,
				outputDir,
				std::format("Temperature vector size at step {} ({}) doesn't match mesh nodes count ({})",
					i, temperatures[i].size(), mesh.GetNodesCount())
			});
		}

		std::string filename = std::format("{}_{:04d}.vtk", baseName, i);
		fs::path filePath = outputDir / filename;

		std::string content = GenerateVTKContent(mesh, temperatures[i], fieldName, timeSteps[i]);
		auto result = fileService.Write(filePath, content);
		if (!result)
		{
			return std::unexpected(MapFileServiceError(result.error()));
		}
	}

	// Write PVD file for ParaView time series
	fs::path pvdPath = outputDir / (baseName + ".pvd");
	std::string pvdContent = GeneratePVDContent(timeSteps, baseName);

	auto pvdResult = fileService.Write(pvdPath, pvdContent);
	if (!pvdResult)
	{
		return std::unexpected(MapFileServiceError(pvdResult.error()));
	}

	LOG_INFO("VTK export completed: {} files + PVD", temperatures.size());
	return {};
}

std::string VTKExporter::GeneratePVDContent(
	const std::vector<double>& timeSteps,
	const std::string& baseName)
{
	std::ostringstream oss;

	oss << "<?xml version=\"1.0\"?>\n";
	oss << "<VTKFile type=\"Collection\" version=\"0.1\">\n";
	oss << "  <Collection>\n";

	for (size_t i = 0; i < timeSteps.size(); i++)
	{
		std::string filename = std::format("{}_{:04d}.vtk", baseName, i);
		oss << std::format("    <DataSet timestep=\"{}\" file=\"{}\"/>\n", timeSteps[i], filename);
	}

	oss << "  </Collection>\n";
	oss << "</VTKFile>\n";

	return oss.str();
}

VTKExportError VTKExporter::MapFileServiceError(const FileServiceError& error)
{
	return VTKExportError{
		VTKExportErrorCode::FileWriteFailed,
		error.path,
		error.message
	};
}

std::string VTKExporter::GenerateVTKContent(
	const mesh::model::Mesh& mesh,
	const Vec& data,
	const std::string& fieldName,
	double time)
{
	std::ostringstream oss;
	oss << std::setprecision(10);

	const auto& nodes = mesh.GetNodes();
	const auto& quads = mesh.GetQuads();

	// VTK Legacy Header
	oss << "# vtk DataFile Version 3.0\n";
	oss << "FEM Heat Transfer Solution";
	if (time > 0.0)
	{
		oss << " (t=" << time << "s)";
	}
	oss << "\n";
	oss << "ASCII\n";
	oss << "DATASET UNSTRUCTURED_GRID\n";

	// Points
	oss << "POINTS " << nodes.size() << " double\n";
	for (const auto& node : nodes)
	{
		oss << node.x << " " << node.y << " 0.0\n";
	}

	// Cells (Quads)
	size_t cellDataSize = quads.size() * 5; // 4 nodes + 1 count per quad
	oss << "CELLS " << quads.size() << " " << cellDataSize << "\n";

	for (const auto& quad : quads)
	{
		oss << "4";
		for (int i = 0; i < 4; i++)
		{
			size_t localIdx = mesh.GetNodeLocalId(quad.nodeIDs[i]);
			oss << " " << localIdx;
		}
		oss << "\n";
	}

	// Cell types (9 = VTK_QUAD)
	oss << "CELL_TYPES " << quads.size() << "\n";
	for (size_t i = 0; i < quads.size(); i++)
	{
		oss << "9\n";
	}

	// Point data (temperature)
	oss << "POINT_DATA " << nodes.size() << "\n";
	oss << "SCALARS " << fieldName << " double 1\n";
	oss << "LOOKUP_TABLE default\n";

	for (Eigen::Index i = 0; i < data.size(); i++)
	{
		oss << data(i) << "\n";
	}

	return oss.str();
}

} // namespace fem::fileio
