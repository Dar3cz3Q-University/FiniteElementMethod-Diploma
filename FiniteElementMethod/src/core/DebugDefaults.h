#pragma once

#ifdef _DEBUG

#include "ApplicationOptions.h"

namespace fem::core::debug
{

inline ApplicationOptions GetQuickTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\quick.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	return opts;
}

inline ApplicationOptions GetTransientTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\transient.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	return opts;
}

inline ApplicationOptions GetLargeTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\mesh\\test.msh";
	opts.LinearSolverType = solver::linear::LinearSolverType::SparseQR;
	opts.numberOfThreads = 8;
	return opts;
}

} // namespace fem::core::debug

#endif
