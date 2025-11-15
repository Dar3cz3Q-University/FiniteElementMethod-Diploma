#pragma once

#include "math/math.h"

namespace fem::domain
{

struct ElementMatrices
{
	Mat4 H;
	Mat4 C;
	Vec4 P;
};

}
