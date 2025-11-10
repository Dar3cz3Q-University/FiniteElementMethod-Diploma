#include "CliParser.h"

#include <exception>
#include <iostream>

#include <cxxopts.hpp>

namespace fem::cli
{

std::expected<fem::core::ApplicationOptions, CliError> CliParser::Parse(int argc, const char* const* argv)
{
	cxxopts::Options options("Finite Element Method", "Description - to be filled :)");

	options.add_options()
		("h,help", "Help page")
		("input", "Input file", cxxopts::value<std::string>());

	cxxopts::ParseResult result;
	try
	{
		result = options.parse(argc, argv);
	}
	catch (const cxxopts::exceptions::parsing& err)
	{
		return std::unexpected(CliError{ CliErrorCode::ParsingError, err.what() });
	}

	fem::core::ApplicationOptions aptOpts{};

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		aptOpts.ShowHelp = result["help"].as<bool>();
		return aptOpts;
	}

	if (result.count("input"))
	{
		aptOpts.InputPath = result["input"].as<std::string>();
	}

	return aptOpts;
}

}
