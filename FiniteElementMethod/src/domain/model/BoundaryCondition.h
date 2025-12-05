#pragma once

#include "BoundaryConditionType.h"

#include <optional>
#include <string>

namespace fem::domain::model
{

struct BoundaryCondition
{
	std::string physicalGroupName;
	BoundaryConditionType type;

	std::optional<double> temperature; // For <see cref="BoundaryConditionType.Temperature" />
	std::optional<double> heatFlux; // For <see cref="BoundaryConditionType.Flux" />
	std::optional<double> alpha; // For <see cref="BoundaryConditionType.Convection" />
	std::optional<double> ambientTemperature; // For <see cref="BoundaryConditionType.Convection" />
};

} // namespace fem::domain::model
