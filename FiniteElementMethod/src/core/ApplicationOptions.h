#pragma once

#include <filesystem>
#include <optional>

namespace fem::core {

struct ApplicationOptions
{
	bool ShowHelp = false;
	std::filesystem::path InputPath;
};

}
