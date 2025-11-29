#pragma once

#include "IntegrationSchema.h"

#include <vector>

namespace fem::domain::integration
{

/// <summary>
/// Stores precomputed data for Gauss-Legendre integration over a 2-node line
/// element in natural coordinates (-).
/// </summary>
struct LineIntegrationData
{
    /// <summary>
    /// Total number of Gauss points used in the integration along the line.
    /// </summary>
    int nPoints{};

    /// <summary>
    /// --coordinates of integration points in the reference element [-1, 1].
    /// </summary>
    std::vector<double> ksi;

    /// <summary>
    /// Integration weights associated with each Gauss point.
    /// </summary>
    std::vector<double> weights;

    /// <summary>
    /// Shape function values N1, N2 evaluated at each integration point.
    /// Each array corresponds to one Gauss point and contains values for 2 nodes.
    /// </summary>
    std::vector<std::array<double, 2>> N;

    /// <summary>
    /// Partial derivatives of shape functions with respect to - (dN/d-)
    /// evaluated at each integration point. Each array contains derivatives for 2 nodes.
    /// For linear elements: dN1/d- = -0.5, dN2/d- = 0.5 (constant).
    /// </summary>
    std::vector<std::array<double, 2>> dN_dKsi;

    /// <summary>
    /// Precomputed N-N^T matrices for each integration point.
    /// This is used for boundary condition assembly (e.g., convection term).
    /// Each matrix is 2-2 representing outer product of shape functions.
    /// </summary>
    std::vector<std::array<std::array<double, 2>, 2>> N_N_T;
};

LineIntegrationData BuildLineIntegrationData(IntegrationSchema schema);  // TODO: Create custom error type

}
