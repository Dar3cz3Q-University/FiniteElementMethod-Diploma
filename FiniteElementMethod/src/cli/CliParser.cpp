#include "CliParser.h"

#include "core/core.h"

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
		("mesh", "Input mesh file", cxxopts::value<std::string>())
		("nthreads", "Number of threads", cxxopts::value<std::size_t>());

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

	if (result.count("mesh"))
	{
		aptOpts.MeshInputPath = result["input"].as<std::string>();
	}

	if (result.count("nthreads"))
	{
		aptOpts.NumberOfThreads = result["nthreads"].as<std::size_t>();
	}

	return aptOpts;
}

} // namespace fem::cli
