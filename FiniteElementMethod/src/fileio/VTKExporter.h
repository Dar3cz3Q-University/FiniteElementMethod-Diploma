#pragma once

#include "FileService.h"
#include "VTKExportError.h"
#include "mesh/model/Mesh.h"
#include "math/math.h"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace fem::fileio
{

namespace fs = std::filesystem;

class VTKExporter
{
public:
	static std::expected<void, VTKExportError> ExportSteady(
		const fs::path& outputPath,
		const mesh::model::Mesh& mesh,
		const Vec& temperature,
		const std::string& fieldName = "Temperature");

	static std::expected<void, VTKExportError> ExportTransient(
		const fs::path& outputDir,
		const mesh::model::Mesh& mesh,
		const std::vector<Vec>& temperatures,
		const std::vector<double>& timeSteps,
		const std::string& baseName = "solution",
		const std::string& fieldName = "Temperature");

private:
	static std::string GenerateVTKContent(
		const mesh::model::Mesh& mesh,
		const Vec& data,
		const std::string& fieldName,
		double time);

	static std::string GeneratePVDContent(
		const std::vector<double>& timeSteps,
		const std::string& baseName);

	static VTKExportError MapFileServiceError(const FileServiceError& error);
};

} // namespace fem::fileio
