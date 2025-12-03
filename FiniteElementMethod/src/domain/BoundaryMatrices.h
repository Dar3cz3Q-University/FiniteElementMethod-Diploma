#pragma once

#include "math/math.h"

namespace fem::domain
{

/// <summary>
/// Contains boundary-condition-related matrices and vectors for a single
/// boundary segment of a finite element (typically for Robin or Neumann conditions).
/// </summary>
struct BoundaryMatrices
{
    /// <summary>
    /// Boundary conductivity matrix H (2-2),
    /// assembled from integration along an element edge.
    /// </summary>
    Mat2 H;

    /// <summary>
    /// Boundary load vector P (2-1),
    /// representing contributions from boundary conditions.
    /// </summary>
    Vec2 P;
};

}
