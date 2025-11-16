#pragma once

#include <vector>

namespace fem::domain::integration
{

/// <summary>
/// Precomputed Gauss–Legendre quadrature points on the interval [-1, 1].
/// Each inner vector contains the quadrature points for a given order:
/// index 0 -> 1-point rule, index 1 -> 2-point rule, ..., index 4 -> 5-point rule.
/// </summary>
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

/// <summary>
/// Precomputed Gauss–Legendre quadrature weights corresponding to <see cref="LEGENDRE_POINTS"/>.
/// Each inner vector contains the weights for a given order:
/// index 0 -> 1-point rule, index 1 -> 2-point rule, ..., index 4 -> 5-point rule.
/// </summary>
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
