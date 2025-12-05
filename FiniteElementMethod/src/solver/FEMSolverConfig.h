#pragma once

#include "domain/domain.h"
#include "linear/linear.h"

#include <optional>

namespace fem::solver
{

struct FEMSolverConfig
{
	domain::model::ProblemType problemType;

	linear::LinearSolverType linearSolver;

	std::optional<domain::model::TransientConfig> transientConfig;
};

} // namespace fem::solver
