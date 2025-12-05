#pragma once

#include <string>

namespace fem::domain::model
{

/// <summary>
/// Represents material properties used in finite element method (FEM) simulations.
/// All physical quantities are expressed in SI units.
/// </summary>
struct Material
{
	/// <summary>
	/// Unique name or identifier of the material (dimensionless).
	/// </summary>
	std::string name;

	/// <summary>
	/// Thermal conductivity of the material [W/(m*K)].
	/// </summary>
	double conductivity;

	/// <summary>
	/// Density of the material [kg/m^3].
	/// </summary>
	double density;

	/// <summary>
	/// Specific heat capacity of the material [J/(kg*K)].
	/// </summary>
	double specificHeat;
};

} // namespace fem::domain::model
