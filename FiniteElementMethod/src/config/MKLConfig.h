#pragma once

#include "logger/logger.h"

#ifdef EIGEN_USE_MKL_ALL
#include <mkl.h>
#endif

namespace fem::config
{

class MKLConfig
{
public:
	static constexpr bool IsEnabled()
	{
#ifdef EIGEN_USE_MKL_ALL
		return true;
#else
		return false;
#endif
	}

	static void SetNumThreads(int nThreads)
	{
		if constexpr (IsEnabled())
		{
			mkl_set_num_threads(nThreads);
			LOG_INFO("MKL: Set to {} threads", nThreads);
		}
		else
		{
			LOG_WARN("MKL not enabled");
		}
	}

	static int GetMaxThreads()
	{
		if constexpr (IsEnabled())
		{
			return mkl_get_max_threads();
		}
		else
		{
			return 1;
		}
	}

	static int GetAvailableHardwareThreads()
	{
		if constexpr (IsEnabled())
		{
			return mkl_get_max_threads();
		}
		else
		{
			return 1;
		}
	}

	static void SetDynamic(bool enable)
	{
		if constexpr (IsEnabled())
		{
			mkl_set_dynamic(enable ? 1 : 0);
			LOG_INFO("MKL: Dynamic threading {}", enable ? "enabled" : "disabled");
		}
		else
		{
			LOG_WARN("MKL not enabled");
		}
	}

	static bool IsDynamic()
	{
		if constexpr (IsEnabled())
		{
			return mkl_get_dynamic() != 0;
		}
		else
		{
			return false;
		}
	}

	static void PrintInfo()
	{
		LOG_INFO("MKL Configuration:");

		if constexpr (IsEnabled())
		{
			LOG_INFO("  Status: Enabled");
			LOG_INFO("  Max threads: {}", GetMaxThreads());
			LOG_INFO("  Dynamic threading: {}", IsDynamic() ? "enabled" : "disabled");
			LOG_INFO("  Hardware threads: {}", GetAvailableHardwareThreads());

			MKLVersion version;
			mkl_get_version(&version);
			LOG_INFO("  MKL version: {}.{}.{}", version.MajorVersion, version.MinorVersion, version.UpdateVersion);
		}
		else
		{
			LOG_INFO("  Status: Disabled (using Eigen native)");
		}
	}

private:
	MKLConfig() = delete;
};

}
