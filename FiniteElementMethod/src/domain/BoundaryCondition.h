#pragma once

#include "BoundaryConditionType.h"

#include <string>

namespace fem::domain
{

struct BoundaryCondition
{
	std::string physicalGroupName;
	BoundaryConditionType Type;
	double ambientTemperature;
	double alpha;
};

}
