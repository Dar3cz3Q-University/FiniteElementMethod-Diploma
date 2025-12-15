#pragma once

#include <chrono>

namespace fem::metrics
{

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Clock = std::chrono::high_resolution_clock;

inline TimePoint Now()
{
	return Clock::now();
}

inline double ElapsedMs(TimePoint start, TimePoint end)
{
	return std::chrono::duration<double, std::milli>(end - start).count();
}

inline double ElapsedUs(TimePoint start, TimePoint end)
{
	return std::chrono::duration<double, std::micro>(end - start).count();
}

inline double ElapsedSec(TimePoint start, TimePoint end)
{
	return std::chrono::duration<double>(end - start).count();
}

}
