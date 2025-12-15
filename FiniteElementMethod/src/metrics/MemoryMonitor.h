#pragma once

namespace fem::metrics
{

class MemoryMonitor
{
public:
	static size_t GetCurrentUsage();
	static size_t GetPeakUsage();

	static double ToMB(size_t bytes)
	{
		return bytes / (1024.0 * 1024.0);
	};

private:
	static size_t GetWindowsCurrentUsage();
	static size_t GetWindowsPeakUsage();
	static size_t GetLinuxCurrentUsage();
	static size_t GetLinuxPeakUsage();
};

}
