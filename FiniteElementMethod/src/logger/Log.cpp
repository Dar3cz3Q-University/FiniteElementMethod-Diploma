#include "Log.h"

namespace fem {
namespace logger {

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init() {
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s]: %v");
	s_Logger = spdlog::stdout_color_mt("FEM");
	s_Logger->set_level(spdlog::level::trace);

	// TODO: Add logging to file

	LOG_TRACE("Logger initialized");
}

} // namespace log
} // namespace fem
