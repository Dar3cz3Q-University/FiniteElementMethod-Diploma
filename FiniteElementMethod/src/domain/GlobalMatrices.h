#pragma once

#include "math/math.h"

namespace fem::domain
{

struct GlobalMatrices
{
	SpMat H;
	SpMat C;
	Vec P;
};

}
