#include "QuadIntegrationData.h"

#include "LegendreValues.h"

#include "logger/logger.h"

#include <expected>

namespace fem::domain::integration
{

QuadIntegrationData BuildQuadIntegrationData(IntegrationSchema schema)
{
    LOG_TRACE("[{}]: schema = {}", __func__, std::to_underlying(schema));

    const int nGauss = std::to_underlying(schema);

    if (nGauss <= 0 || static_cast<std::size_t>(nGauss) > LEGENDRE_POINTS.size())
    {
        // TODO: Return error
    }

    const auto& points1D = LEGENDRE_POINTS.at(static_cast<std::size_t>(nGauss - 1));
    const auto& weights1D = LEGENDRE_WEIGHTS.at(static_cast<std::size_t>(nGauss - 1));

    QuadIntegrationData out;
    out.nGauss = nGauss;
    out.nPoints = nGauss * nGauss;

    out.ksi.reserve(out.nPoints);
    out.eta.reserve(out.nPoints);
    out.weights.reserve(out.nPoints);

    out.N.reserve(out.nPoints);
    out.dN_dKsi.reserve(out.nPoints);
    out.dN_dEta.reserve(out.nPoints);

    int gpIndex = 0;
    for (int i = 0; i < nGauss; i++) for (int j = 0; j < nGauss; j++)
    {
        const double ksi = points1D.at(i);
        const double eta = points1D.at(j);
        const double w = weights1D.at(i) * weights1D.at(j);

        out.ksi.push_back(ksi);
        out.eta.push_back(eta);
        out.weights.push_back(w);

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

        out.N.push_back(Nvals);
        out.dN_dKsi.push_back(dKsi);
        out.dN_dEta.push_back(dEta);

        LOG_TRACE("IP = {}, ksi = {:.6f}, eta = {:.6f}, w = {:.6f}", gpIndex++, ksi, eta, w);
        LOG_TRACE("dN/dKsi = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", dKsi[0], dKsi[1], dKsi[2], dKsi[3]);
        LOG_TRACE("dN/dEta = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", dEta[0], dEta[1], dEta[2], dEta[3]);
        LOG_TRACE("N = [{:.6f}, {:.6f}, {:.6f}, {:.6f}]", Nvals[0], Nvals[1], Nvals[2], Nvals[3]);
    }

    return out;
}

}
