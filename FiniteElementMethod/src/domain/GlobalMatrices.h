#pragma once

#include "math/math.h"

namespace fem::domain
{

struct GlobalMatrices
{
	Mat H;
	Mat C;
	Vec P;
};

}
