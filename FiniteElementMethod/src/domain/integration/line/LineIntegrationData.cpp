#include "LineIntegrationData.h"

#include "../LegendreValues.h"

#include "logger/logger.h"

namespace fem::domain::integration
{

std::expected<LineIntegrationData, IntegrationError> BuildLineIntegrationData(IntegrationSchema schema)
{
	LOG_TRACE("[{}]: schema = {}", __func__, std::to_underlying(schema));

	const int nGauss = std::to_underlying(schema);
	if (nGauss <= 0 || static_cast<std::size_t>(nGauss) > LEGENDRE_POINTS.size())
	{
		return std::unexpected(
			IntegrationError{
				IntegrationErrorCode::IncorrectIntegrationSchema,
				"Unsupported integration schema: " + std::to_string(nGauss)
			}
		);
	}

	const auto& points1D = LEGENDRE_POINTS.at(static_cast<std::size_t>(nGauss - 1));
	const auto& weights1D = LEGENDRE_WEIGHTS.at(static_cast<std::size_t>(nGauss - 1));

	LineIntegrationData out;
	out.nPoints = nGauss;

	out.ksi.reserve(out.nPoints);
	out.weights.reserve(out.nPoints);
	out.N.reserve(out.nPoints);
	out.dN_dKsi.reserve(out.nPoints);
	out.N_N_T.reserve(out.nPoints);

	for (int i = 0; i < nGauss; i++)
	{
		const double ksi = points1D.at(i);
		const double w = weights1D.at(i);

		out.ksi.push_back(ksi);
		out.weights.push_back(w);

		std::array<double, 2> Nvals = {
			0.5 * (1.0 - ksi),
			0.5 * (1.0 + ksi)
		};

		std::array<double, 2> dKsi = {
			-0.5,
			 0.5
		};

		std::array<std::array<double, 2>, 2> N_N_T_matrix = { {
			{Nvals[0] * Nvals[0], Nvals[0] * Nvals[1]},
			{Nvals[1] * Nvals[0], Nvals[1] * Nvals[1]}
		} };

		out.N.push_back(Nvals);
		out.dN_dKsi.push_back(dKsi);
		out.N_N_T.push_back(N_N_T_matrix);

		LOG_TRACE("IP = {}, ksi = {:.6f}, w = {:.6f}", i, ksi, w);
		LOG_TRACE("N = [{:.6f}, {:.6f}]", Nvals[0], Nvals[1]);
		LOG_TRACE("dN/dKsi = [{:.6f}, {:.6f}]", dKsi[0], dKsi[1]);
		LOG_TRACE("N*N^T = [[{:.6f}, {:.6f}], [{:.6f}, {:.6f}]]",
			N_N_T_matrix[0][0], N_N_T_matrix[0][1],
			N_N_T_matrix[1][0], N_N_T_matrix[1][1]);
	}

	return out;
}

}
