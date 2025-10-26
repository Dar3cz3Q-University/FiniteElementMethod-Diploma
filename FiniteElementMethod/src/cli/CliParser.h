#pragma once

#include "CliError.h"
#include "core/ApplicationOptions.h"

#include <expected>

namespace fem::cli
{

class CliParser
{
public:
	CliParser() = default;

	static std::expected<fem::core::ApplicationOptions, CliError> Parse(int argc, const char* const* argv);
};

}
