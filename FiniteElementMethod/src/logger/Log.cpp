#include "Log.h"

#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace fem::logger {

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init() {
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s]: %v");
	s_Logger = spdlog::stdout_color_mt("FEM");
	s_Logger->set_level(spdlog::level::trace);

	// TODO: Add logging to file

	LOG_TRACE("Logger initialized");
}

}
