#pragma once

#include "ILinearSolver.h"
#include "LinearSolverType.h"

#include <memory>

namespace fem::solver::linear
{

/// <summary>
/// Factory for creating linear solver instances
/// </summary>
class LinearSolverFactory
{
public:
	/// <summary>
	/// Create solver based on type
	/// </summary>
	static std::unique_ptr<ILinearSolver> Create(LinearSolverType type);
};

} // namespace fem::solver::linear
