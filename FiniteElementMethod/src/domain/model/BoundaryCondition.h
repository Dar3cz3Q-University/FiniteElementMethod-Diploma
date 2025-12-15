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

	std::optional<double> temperature;
	std::optional<double> heatFlux;
	std::optional<double> alpha;
	std::optional<double> ambientTemperature;
};

} // namespace fem::domain::model
