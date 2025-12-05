#pragma once

#ifdef _DEBUG

#include "ApplicationOptions.h"

namespace fem::core::debug
{

inline ApplicationOptions GetSteadyTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\steady.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.logLevel = spdlog::level::trace;
	return opts;
}

inline ApplicationOptions GetTransientTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\transient.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.logLevel = spdlog::level::info;
	return opts;
}

inline ApplicationOptions GetLargeTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\large.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.numberOfThreads = 16;
	return opts;
}

} // namespace fem::core::debug

#endif
