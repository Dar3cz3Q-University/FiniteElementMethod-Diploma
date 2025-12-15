#pragma once

constexpr double BytesToMiB(size_t bytes) noexcept
{
	return static_cast<double>(bytes) / (1024.0 * 1024.0);
}
