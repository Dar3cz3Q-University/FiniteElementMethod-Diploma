#pragma once

#include "IntegrationSchema.h"

#include <array>
#include <vector>

namespace fem::domain::integration
{

/// <summary>
/// Stores precomputed data for Gauss–Legendre integration over a 4-node quadrilateral
/// element in natural coordinates (ξ, η).
/// </summary>
struct QuadIntegrationData
{
    /// <summary>
    /// Total number of Gauss points used in the integration (product of points per direction).
    /// </summary>
    int nGauss{};

    /// <summary>
    /// Number of integration points per direction (ξ and η).
    /// </summary>
    int nPoints{};

    /// <summary>
    /// ξ-coordinates of integration points in the reference element [-1, 1] × [-1, 1].
    /// </summary>
    std::vector<double> ksi;

    /// <summary>
    /// η-coordinates of integration points in the reference element [-1, 1] × [-1, 1].
    /// </summary>
    std::vector<double> eta;

    /// <summary>
    /// Integration weights associated with each Gauss point.
    /// </summary>
    std::vector<double> weights;

    /// <summary>
    /// Shape function values N1..N4 evaluated at each integration point.
    /// Each array corresponds to one Gauss point and contains values for 4 nodes.
    /// </summary>
    std::vector<std::array<double, 4>> N;

    /// <summary>
    /// Partial derivatives of shape functions with respect to ξ (dN/dξ)
    /// evaluated at each integration point. Each array contains derivatives for 4 nodes.
    /// </summary>
    std::vector<std::array<double, 4>> dN_dKsi;

    /// <summary>
    /// Partial derivatives of shape functions with respect to η (dN/dη)
    /// evaluated at each integration point. Each array contains derivatives for 4 nodes.
    /// </summary>
    std::vector<std::array<double, 4>> dN_dEta;
};

/// <summary>
/// Builds quadrilateral integration data (Gauss points, weights, shape functions and
/// their derivatives) for a given Gauss–Legendre integration scheme.
/// </summary>
/// <param name="schema">
/// Integration scheme specifying the number of Gauss points per direction.
/// </param>
/// <returns>
/// A fully initialized <see cref="QuadIntegrationData"/> structure for the given scheme.
/// </returns>
QuadIntegrationData BuildQuadIntegrationData(IntegrationSchema schema); // TODO: Create custom error type

}
