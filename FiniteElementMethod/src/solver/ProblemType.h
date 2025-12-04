#pragma once

#include <array>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>

namespace fem::solver
{

enum class ProblemType : int
{
	Steady = 0,
	Transient,
};

struct ProblemTypeInfo
{
	ProblemType type;
	std::string name;
	std::string description;
};

inline std::optional<ProblemType> ParseProblemType(std::string_view str)
{
	using enum ProblemType;

	if (str == "steady") return Steady;
	else if (str == "transient") return Transient;

	return std::nullopt;
}

inline static const std::array<ProblemTypeInfo, 2> PROBLEM_TYPES = { {
	{ ProblemType::Steady,    "steady",    "Steady-state heat transfer" },
	{ ProblemType::Transient, "transient", "Time-dependent heat transfer" }
} };

} // namespace fem::solver
