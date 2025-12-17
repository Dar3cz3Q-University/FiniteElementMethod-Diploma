#include "QuadIntegrationData.h"

#include "../LegendreValues.h"

#include "logger/logger.h"

#include <format>

namespace fem::domain::integration
{

std::expected<QuadIntegrationData, IntegrationError> BuildQuadIntegrationData(IntegrationSchema schema)
{
	LOG_TRACE("[{}]: schema = {}", __func__, std::to_underlying(schema));

	const int nGauss = std::to_underlying(schema);

	if (nGauss <= 0 || static_cast<std::size_t>(nGauss) > LEGENDRE_POINTS.size())
	{
		return std::unexpected(
			IntegrationError{
				IntegrationErrorCode::IncorrectIntegrationSchema,
				std::format("Unsupported integration schema: {}", std::to_string(nGauss))
			}
		);
	}

	const auto& points1D = LEGENDRE_POINTS.at(static_cast<std::size_t>(nGauss - 1));
	const auto& weights1D = LEGENDRE_WEIGHTS.at(static_cast<std::size_t>(nGauss - 1));

	QuadIntegrationData out;
	out.nGauss = nGauss;
	out.nPoints = nGauss * nGauss;

	int gpIndex = 0;
	for (int i = 0; i < nGauss; i++) for (int j = 0; j < nGauss; j++)
	{
		const double ksi = points1D[i];
		const double eta = points1D[j];
		const double w = weights1D[i] * weights1D[j];

		out.ksi[gpIndex] = ksi;
		out.eta[gpIndex] = eta;
		out.weights[gpIndex] = w;

		std::array<double, 4> Nvals = {
			0.25 * (1.0 - ksi) * (1.0 - eta),
			0.25 * (1.0 + ksi) * (1.0 - eta),
			0.25 * (1.0 + ksi) * (1.0 + eta),
			0.25 * (1.0 - ksi) * (1.0 + eta)
		};

		std::array<double, 4> dKsi = {
			-0.25 * (1.0 - eta),
			0.25 * (1.0 - eta),
			0.25 * (1.0 + eta),
			-0.25 * (1.0 + eta)
		};

		std::array<double, 4> dEta = {
			-0.25 * (1.0 - ksi),
			-0.25 * (1.0 + ksi),
			0.25 * (1.0 + ksi),
			0.25 * (1.0 - ksi)
		};

		out.N[gpIndex] = Nvals;
		out.dN_dKsi[gpIndex] = dKsi;
		out.dN_dEta[gpIndex] = dEta;

		// Precompute N * N^T for capacity matrix
		for (int a = 0; a < 4; a++)
		{
			for (int b = 0; b < 4; b++)
			{
				out.N_N_T[gpIndex][a][b] = Nvals[a] * Nvals[b];
			}
		}

		LOG_TRACE("IP = {}, ksi = {:.6f}, eta = {:.6f}, w = {:.6f}", gpIndex, ksi, eta, w);
		LOG_TRACE("dN/dKsi = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", dKsi[0], dKsi[1], dKsi[2], dKsi[3]);
		LOG_TRACE("dN/dEta = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", dEta[0], dEta[1], dEta[2], dEta[3]);
		LOG_TRACE("N = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", Nvals[0], Nvals[1], Nvals[2], Nvals[3]);

		gpIndex++;
	}

	return out;
}

}
