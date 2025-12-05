#pragma once

#include "math/math.h"

namespace fem::domain::model
{

struct TransientConfig
{
	double totalTime;
	double timeStep;
	Vec initialConditions;
};

} // namespace fem::domain::model
