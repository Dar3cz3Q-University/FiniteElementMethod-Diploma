#include "MemoryMonitor.h"

#include "platform/platform.h"

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

#ifdef __linux__
#include <sstream>
#include <string>
#include "FileIO.h"
#endif

namespace fem::metrics
{

size_t fem::metrics::MemoryMonitor::GetCurrentUsage()
{
	if constexpr (IS_WINDOWS)
	{
		return GetWindowsCurrentUsage();
	}
	else if constexpr (IS_LINUX)
	{
		return GetLinuxCurrentUsage();
	}
	else
	{
		return 0;
	}
}

size_t MemoryMonitor::GetPeakUsage()
{
	if constexpr (IS_WINDOWS)
	{
		return GetWindowsPeakUsage();
	}
	else if constexpr (IS_LINUX)
	{
		return GetLinuxPeakUsage();
	}
	else
	{
		return 0;
	}
}

#ifdef _WIN32

size_t MemoryMonitor::GetWindowsCurrentUsage()
{
	PROCESS_MEMORY_COUNTERS_EX pmc{};
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		return pmc.WorkingSetSize;
	}

	return 0;
}

size_t MemoryMonitor::GetWindowsPeakUsage()
{
	PROCESS_MEMORY_COUNTERS_EX pmc{};
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
	{
		return pmc.PeakWorkingSetSize;
	}
	return 0;
}

#endif

#ifdef __linux__

size_t MemoryMonitor::GetLinuxCurrentUsage()
{
	auto fileService = fileio::FileService();
	const auto& readData = fileService.Read("/proc/self/status");

	if (!readData) return 0;

	std::istringstream stream(*readData);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.substr(0, 6) == "VmRSS:")
		{
			std::istringstream iss(line.substr(6));
			size_t mem_kb;
			iss >> mem_kb;
			return mem_kb * 1024;
		}
	}
	return 0;
}

size_t MemoryMonitor::GetLinuxPeakUsage()
{
	auto fileService = fileio::FileService();
	const auto& readData = fileService.Read("/proc/self/status");

	if (!readData) return 0;

	std::istringstream stream(*readData);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.substr(0, 7) == "VmPeak:")
		{
			std::istringstream iss(line.substr(7));
			size_t mem_kb;
			iss >> mem_kb;
			return mem_kb * 1024;
		}
	}
	return 0;
}

#endif

}
