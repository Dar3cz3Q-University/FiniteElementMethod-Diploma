#pragma once

#include <filesystem>
#include <optional>

namespace fem::core {

struct ApplicationOptions
{
	std::filesystem::path InputPath;
	std::optional<double> Lc;
	bool ShowHelp = false;
	bool Verbose = false;
	std::optional<std::filesystem::path> ExportMeshPath;
	std::optional<std::filesystem::path> ExportResultPath;
};

}
