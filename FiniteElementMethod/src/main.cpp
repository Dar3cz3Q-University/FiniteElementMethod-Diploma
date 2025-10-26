#include "core/Application.h"
#include "core/ExitCode.h"
#include "cli/CliParser.h"

#include <iostream>
#include <vector>
#include <print>

int main(int argc, const char* const* argv) {
	auto options = fem::cli::CliParser::Parse(argc, argv);

	if (!options)
	{
		std::println("{}", options.error());
		return std::to_underlying(fem::core::ExitCode::CliError);
	}

	fem::core::Application app(*options);
	auto status = app.Execute();
	return std::to_underlying(status);
}
