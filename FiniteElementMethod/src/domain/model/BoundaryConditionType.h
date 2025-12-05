#pragma once

namespace fem::domain::model
{

/// <summary>
/// Types of boundary conditions supported by the FEM solver.
/// </summary>
enum class BoundaryConditionType : int
{
	/// <summary>
	/// Prescribed temperature boundary condition.
	/// Dirichlet: T = value
	/// </summary>
	Temperature,

	/// <summary>
	/// Prescribed heat flux boundary condition.
	/// Neumann: q = value
	/// </summary>
	Flux,

	/// <summary>
	/// Convection boundary condition.
	/// Robin: q = h*(T_ambient - T)
	/// </summary>
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
