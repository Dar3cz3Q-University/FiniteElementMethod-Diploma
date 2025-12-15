#pragma once

namespace fem::domain::model
{

enum class BoundaryConditionType : int
{
	Temperature,
	Flux,
	Convection,
};

inline std::optional<BoundaryConditionType> ParseBoundaryConditionType(std::string_view str)
{
	using enum BoundaryConditionType;

	if (str == "temperature") return Temperature;
	else if (str == "flux") return Flux;
	else if (str == "convection") return Convection;

	return std::nullopt;
}

} // namespace fem::domain::model
