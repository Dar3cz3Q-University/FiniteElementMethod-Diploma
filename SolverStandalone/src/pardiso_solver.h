#pragma once

#include "types.h"

namespace fem::solver::standalone
{

bool SolvePARDISO_LDLT(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats);
bool SolvePARDISO_LU(const SpMat& K, const Vec& b, Vec& x, SolverStats& stats);
void PrintBenchmark(const SolverStats& stats);
void PrintMKLInfo();

} // namespace fem::solver::standalone
