#pragma once

#include "domain/domain.h"

#include <optional>
#include <vector>

namespace fem::config
{

struct ProblemConfig
{
	std::filesystem::path meshPath;
	domain::model::ProblemType problemType;
	std::optional<domain::model::TransientConfig> transientConfig;
	domain::model::Material material;
	domain::model::BoundaryCondition boundaryCondition; // TODO: Support multiple BCs
};

} // namespace fem::config
