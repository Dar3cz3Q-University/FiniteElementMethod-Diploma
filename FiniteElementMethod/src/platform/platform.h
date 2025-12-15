#pragma once

#ifdef _WIN32
constexpr bool IS_WINDOWS = true;
constexpr bool IS_LINUX = false;
#elif __linux__
constexpr bool IS_WINDOWS = false;
constexpr bool IS_LINUX = true;
#else
constexpr bool IS_WINDOWS = false;
constexpr bool IS_LINUX = false;
#endif
