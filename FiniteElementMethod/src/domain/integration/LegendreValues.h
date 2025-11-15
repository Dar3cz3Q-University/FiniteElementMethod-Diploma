#pragma once

#include <vector>

namespace fem::domain::integration
{

// Gauss–Legendre quadrature points and weights on the reference interval [-1, 1].
//
// LEGENDRE_POINTS[n - 1]  -> 1D Gauss points for an n-point quadrature rule
// LEGENDRE_WEIGHTS[n - 1] -> corresponding 1D Gauss weights for the same rule
//
// All values are defined on the reference domain and are independent of the mesh
// geometry. They are reused for every element in the mesh.

inline static const std::vector<std::vector<double>> LEGENDRE_POINTS = {
    { 0.0 },
    { -sqrt(1.0 / 3.0), sqrt(1.0 / 3.0) },
    { -sqrt(3.0 / 5.0), 0.0, sqrt(3.0 / 5.0) },
    {
        -sqrt((3.0 / 7.0) + (2.0 / 7.0) * sqrt(6.0 / 5.0)),
        -sqrt((3.0 / 7.0) - (2.0 / 7.0) * sqrt(6.0 / 5.0)),
        sqrt((3.0 / 7.0) - (2.0 / 7.0) * sqrt(6.0 / 5.0)),
        sqrt((3.0 / 7.0) + (2.0 / 7.0) * sqrt(6.0 / 5.0))
    },
    {
        -((1.0 / 3.0) * sqrt(5.0 + 2.0 * sqrt(10.0 / 7.0))),
        -((1.0 / 3.0) * sqrt(5.0 - 2.0 * sqrt(10.0 / 7.0))),
        0.0,
        ((1.0 / 3.0) * sqrt(5.0 - 2.0 * sqrt(10.0 / 7.0))),
        ((1.0 / 3.0) * sqrt(5.0 + 2.0 * sqrt(10.0 / 7.0)))
    }
};

inline static const std::vector<std::vector<double>> LEGENDRE_WEIGHTS = {
    { 2.0 },
    { 1.0, 1.0 },
    { 5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0 },
    {
        (18.0 - sqrt(30.0)) / 36.0,
        (18.0 + sqrt(30.0)) / 36.0,
        (18.0 + sqrt(30.0)) / 36.0,
        (18.0 - sqrt(30.0)) / 36.0
    },
    {
        ((322.0 - 13.0 * sqrt(70.0)) / 900.0),
        ((322.0 + 13.0 * sqrt(70.0)) / 900.0),
        (128.0 / 225.0),
        ((322.0 + 13.0 * sqrt(70.0)) / 900.0),
        ((322.0 - 13.0 * sqrt(70.0)) / 900.0)
    }
};

}
