#pragma once

#include "logger/logger.h"

#include "gmsh.h"

namespace fem::config
{

class GmshConfig
{
public:

	static void Initialize()
	{
		LOG_INFO("Initializing gmsh");
		gmsh::initialize();
	}

	static void Finalize()
	{
		LOG_INFO("Finalizing gmsh");
		gmsh::finalize();
	}

	static void PrintInfo()
	{
		LOG_INFO("Gmsh Configuration:");

		std::string ver;
		gmsh::option::getString("General.Version", ver);
		LOG_INFO("  Gmsh version: {}", ver);
	}

private:
	GmshConfig() = delete;
};

} // namespace fem::config
