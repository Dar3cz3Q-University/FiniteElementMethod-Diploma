#pragma once

#include "math/math.h"

namespace fem::domain::model
{

struct TransientConfig
{
	double totalTime;
	double timeStep;
	double initialTemperature; // TODO: Change to vector of initial conditions
};

} // namespace fem::domain::model
