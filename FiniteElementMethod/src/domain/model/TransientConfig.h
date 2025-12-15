#pragma once

#include "math/math.h"

#include <optional>

namespace fem::domain::model
{

struct TransientConfig
{
	double totalTime;
	double timeStep;
	bool saveHistory;
	std::optional<size_t> saveStride;
	double initialTemperature; // TODO: Change to vector of initial conditions
};

} // namespace fem::domain::model
