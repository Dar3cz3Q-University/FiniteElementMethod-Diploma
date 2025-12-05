#pragma once

#include <optional>
#include <string>

namespace fem::domain::model
{

enum class ProblemType : int
{
	Steady = 0,
	Transient,
};

inline std::optional<ProblemType> ParseProblemType(std::string_view str)
{
	using enum ProblemType;

	if (str == "steady") return Steady;
	else if (str == "transient") return Transient;

	return std::nullopt;
}

} // fem::domain::model
