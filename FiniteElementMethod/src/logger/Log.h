#pragma once

#include "spdlog/spdlog.h"

namespace fem::logger
{

class Log
{
public:
	/// <summary>
	/// Initialize the logger with specified log level
	/// </summary>
	/// <param name="level">Logging level</param>
	static void Init(spdlog::level::level_enum level);

	/// <summary>
	/// Get the global logger instance
	/// </summary>
	/// <returns>Shared pointer to logger</returns>
	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
	static std::shared_ptr<spdlog::logger> s_Logger;
};

} // namespace fem::logger

#define LOG_TRACE(...)     fem::logger::Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, __VA_ARGS__)
#define LOG_INFO(...)      fem::logger::Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...)      fem::logger::Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...)     fem::logger::Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...)  fem::logger::Log::GetLogger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, __VA_ARGS__)

// TODO: Disable logging in release build
