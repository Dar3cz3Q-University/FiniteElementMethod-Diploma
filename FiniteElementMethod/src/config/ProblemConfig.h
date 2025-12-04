#pragma once

#include "domain/domain.h"

#include <optional>
#include <vector>

namespace fem::config
{

/// <summary>
/// Domain configuration loaded from JSON file.
/// Contains physical problem definition: mesh, material, boundary conditions.
/// </summary>
struct ProblemConfig
{
	/// <summary>
	/// Path to mesh file.
	/// </summary>
	std::filesystem::path meshPath;

	/// <summary>
	/// Problem type.
	/// </summary>
	domain::model::ProblemType problemType;

	/// <summary>
	/// Time integration parameters (required if problemType is Transient)
	/// </summary>
	std::optional<domain::model::TransientConfig> transientConfig;

	/// <summary>
	/// Material properties.
	/// </summary>
	domain::model::Material material;

	/// <summary>
	/// Boundary condition.
	/// </summary>
	domain::model::BoundaryCondition boundaryCondition; // TODO: Support multiple BCs

	/// <summary>
	/// Uniform initial temperature.
	/// </summary>
	double uniformInitialTemperature; // TODO: Think to use Kelvin or Celsius
};

} // namespace fem::config
