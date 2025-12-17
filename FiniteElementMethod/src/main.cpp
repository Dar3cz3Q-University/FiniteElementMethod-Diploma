#include "config/config.h"
#include "core/core.h"
#include "cli/cli.h"

#include <iostream>
#include <vector>
#include <print>

int main(int argc, const char* const* argv)
{
	fem::config::PrintCompileConfig();

	std::expected<fem::core::ApplicationOptions, fem::cli::CliError> options;

#ifdef DEBUG
	auto config = fem::core::debug::GetTransientTest();
	options = config;
#else
	options = fem::cli::CliParser::Parse(argc, argv);
#endif

	if (!options)
	{
		std::println("{}", options.error());
		return std::to_underlying(fem::core::ExitCode::CliError);
	}

	fem::core::Application app(*options);
	auto status = app.Execute();
	return std::to_underlying(status);
}
