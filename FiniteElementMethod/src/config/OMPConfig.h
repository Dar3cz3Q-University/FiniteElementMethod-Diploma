#pragma once

#include "logger/logger.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace fem::config
{

class OMPConfig
{
public:
	static constexpr bool IsEnabled()
	{
#ifdef _OPENMP
		return true;
#else
		return false;
#endif
	}

	static void SetNumThreads(int nThreads)
	{
#ifdef _OPENMP
		omp_set_num_threads(nThreads);
		LOG_INFO("OpenMP: Set to {} threads", nThreads);
#else
		LOG_WARN("OpenMP not enabled");
#endif
	}

	static int GetMaxThreads()
	{
		if constexpr (IsEnabled())
		{
			return omp_get_max_threads();
		}
		else
		{
			return 1;
		}
	}

	static int GetNumProcs()
	{
		if constexpr (IsEnabled())
		{
			return omp_get_num_procs();
		}
		else
		{
			return 1;
		}
	}

	static void PrintInfo()
	{
		LOG_INFO("OpenMP Configuration:");

		if constexpr (IsEnabled())
		{
			LOG_INFO("  Status: Enabled");
			LOG_INFO("  Max threads: {}", GetMaxThreads());
			LOG_INFO("  Available processors: {}", GetNumProcs());
		}
		else
		{
			LOG_INFO("  Status: Disabled");
		}
	}

private:
	OMPConfig() = delete;
};

}
