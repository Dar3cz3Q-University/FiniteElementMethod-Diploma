#pragma once

#include "math/math.h"

namespace fem::domain
{

/// <summary>
/// Contains the element-level matrices and vectors computed for a single
/// 4-node finite element (e.g., in heat transfer or diffusion problems).
/// </summary>
struct ElementMatrices
{
    /// <summary>
    /// Element conductivity matrix H (4-4).
    /// </summary>
    Mat4 H;

    /// <summary>
    /// Element capacity matrix C (4-4).
    /// </summary>
    Mat4 C;

    /// <summary>
    /// Element load vector P (4-1).
    /// </summary>
    Vec4 P;
};

}
