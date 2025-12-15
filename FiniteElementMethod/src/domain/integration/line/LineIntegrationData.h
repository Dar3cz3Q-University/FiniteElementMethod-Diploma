#pragma once

#include "../IntegrationSchema.h"

#include "../IntegrationError.h"

#include <expected>
#include <vector>

namespace fem::domain::integration
{

struct LineIntegrationData
{
	int nPoints{};
	std::vector<double> ksi;
	std::vector<double> weights;
	std::vector<std::array<double, 2>> N;
	std::vector<std::array<double, 2>> dN_dKsi;
	std::vector<std::array<std::array<double, 2>, 2>> N_N_T;
};

std::expected<LineIntegrationData, IntegrationError> BuildLineIntegrationData(IntegrationSchema schema);

}
