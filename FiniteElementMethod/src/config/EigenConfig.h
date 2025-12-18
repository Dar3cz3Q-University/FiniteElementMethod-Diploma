#pragma once

#include "logger/logger.h"

namespace fem::config
{

class EigenConfig
{
public:

	static void PrintInfo()
	{
		LOG_INFO("Eigen Configuration:");
		LOG_INFO("  Eigen version: {}.{}.{}", EIGEN_WORLD_VERSION, EIGEN_MAJOR_VERSION, EIGEN_MINOR_VERSION);
	}

private:
	EigenConfig() = delete;
};

} // namespace fem::config
