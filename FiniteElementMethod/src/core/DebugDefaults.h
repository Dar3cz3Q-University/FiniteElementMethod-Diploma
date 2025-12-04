#pragma once

#ifdef _DEBUG

#include "ApplicationOptions.h"

namespace fem::core::debug
{

inline ApplicationOptions GetQuickTest()
{
	ApplicationOptions opts;
	opts.MeshInputPath = "..\\assets\\mesh\\test.msh";
	opts.ProblemType = solver::ProblemType::Steady;
	opts.SolverType = solver::SolverType::SimplicialLDLT;
	return opts;
}

inline ApplicationOptions GetTransientTest()
{
	ApplicationOptions opts;
	opts.MeshInputPath = "..\\assets\\mesh\\test.msh";
	opts.ProblemType = solver::ProblemType::Transient;
	opts.SolverType = solver::SolverType::SparseLU;
	return opts;
}

inline ApplicationOptions GetLargeTest()
{
	ApplicationOptions opts;
	opts.MeshInputPath = "..\\assets\\mesh\\test.msh";
	opts.ProblemType = solver::ProblemType::Steady;
	opts.SolverType = solver::SolverType::SparseQR;
	opts.NumberOfThreads = 8;
	return opts;
}

} // namespace fem::core::debug

#endif
