#pragma once

#include "IntegrationSchema.h"
#include "LegendreValues.h"

#include <array>
#include <expected>
#include <vector>

namespace fem::domain::integration
{

// Precomputed integration data for a 4-node quadrilateral (Quad4) element
// on the reference domain (ξ, η) ∈ [-1, 1] x [-1, 1].
//
// For a given 1D Gauss order nGauss, we build a tensor-product integration
// rule with nPoints = nGauss * nGauss integration points in 2D.
//
// For each integration point 'gp' (gp = 0 .. nPoints-1) we store:
//
//   - ksi[gp], eta[gp]      : local coordinates (ξ, η)
//   - weights[gp]           : 2D Gauss weight w_ξ * w_η
//   - N[gp][a]              : value of shape function N_a(ξ, η) for node a = 0..3
//   - dN_dKsi[gp][a]        : ∂N_a / ∂ξ at this integration point
//   - dN_dEta[gp][a]        : ∂N_a / ∂η at this integration point
//
// All these values are defined on the reference element only and can be
// reused for every Quad4 element in the mesh.

struct QuadIntegrationData
{
	int nGauss{};
	int nPoints{};

	std::vector<double> ksi;
	std::vector<double> eta;
	std::vector<double> weights;

	std::vector<std::array<double, 4>> N;
	std::vector<std::array<double, 4>> dN_dKsi;
	std::vector<std::array<double, 4>> dN_dEta;
};

// TODO: Create custom error type
QuadIntegrationData BuildQuadIntegrationData(IntegrationSchema schema);

}
