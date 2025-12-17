#pragma once

#include "../IntegrationSchema.h"
#include "../IntegrationError.h"

#include <array>
#include <expected>

namespace fem::domain::integration
{

inline constexpr int MAX_GAUSS_POINTS_1D = 5;
inline constexpr int MAX_GAUSS_POINTS_2D = MAX_GAUSS_POINTS_1D * MAX_GAUSS_POINTS_1D;

struct QuadIntegrationData
{
	int nGauss{};
	int nPoints{};
	std::array<double, MAX_GAUSS_POINTS_2D> ksi{};
	std::array<double, MAX_GAUSS_POINTS_2D> eta{};
	std::array<double, MAX_GAUSS_POINTS_2D> weights{};
	std::array<std::array<double, 4>, MAX_GAUSS_POINTS_2D> N{};
	std::array<std::array<double, 4>, MAX_GAUSS_POINTS_2D> dN_dKsi{};
	std::array<std::array<double, 4>, MAX_GAUSS_POINTS_2D> dN_dEta{};
	std::array<std::array<std::array<double, 4>, 4>, MAX_GAUSS_POINTS_2D> N_N_T{};
};

std::expected<QuadIntegrationData, IntegrationError> BuildQuadIntegrationData(IntegrationSchema schema);

}
