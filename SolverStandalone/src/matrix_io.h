#pragma once

#include "types.h"
#include <string>

namespace fem::solver::standalone
{

bool LoadMatrixMarket(const std::string& filename, SpMat& matrix);
bool LoadVector(const std::string& filename, Vec& vec);
bool SaveVector(const std::string& filename, const Vec& vec);

} // namespace fem::solver::standalone
