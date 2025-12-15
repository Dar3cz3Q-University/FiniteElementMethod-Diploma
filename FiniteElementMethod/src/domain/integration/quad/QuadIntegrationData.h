#pragma once

#include "../IntegrationSchema.h"
#include "../IntegrationError.h"

#include <array>
#include <expected>
#include <vector>

namespace fem::domain::integration
{

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

std::expected<QuadIntegrationData, IntegrationError> BuildQuadIntegrationData(IntegrationSchema schema);

}
