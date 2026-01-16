#pragma once

#ifdef DEBUG

#include "ApplicationOptions.h"

namespace fem::core::debug
{

inline ApplicationOptions GetSteadyTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\steady.json";
	opts.metricsFilePath = "metrics.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.numberOfThreads = 16;
	return opts;
}

inline ApplicationOptions GetTransientTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\transient.json";
	opts.metricsFilePath = "metrics.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.useCache = false;
	return opts;
}

inline ApplicationOptions GetLargeTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\large.json";
	opts.metricsFilePath = "metrics.json";
	opts.LinearSolverType = solver::linear::LinearSolverType::SimplicialLDLT;
	opts.numberOfThreads = 16;
	return opts;
}

inline ApplicationOptions GetBuildMatrixOnlyTest()
{
	ApplicationOptions opts;
	opts.configFilePath = "..\\assets\\config\\steady.json";
	opts.exportMtxPath = "output\\matrices";
	opts.buildMatrixOnly = true;
	opts.numberOfThreads = 16;
	return opts;
}

} // namespace fem::core::debug

#endif
