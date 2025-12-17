#pragma once

#include "FullMetrics.h"

#include <expected>
#include <filesystem>
#include <string>

namespace fem::fileio
{

namespace fs = std::filesystem;

class StatsExporter
{
public:
	static std::expected<void, std::string> Export(const fs::path& path, const FullMetrics& metrics);

private:
	static std::expected<void, std::string> ExportCSV(const fs::path& path, const FullMetrics& metrics);
	static std::expected<void, std::string> ExportJSON(const fs::path& path, const FullMetrics& metrics);
};

}
