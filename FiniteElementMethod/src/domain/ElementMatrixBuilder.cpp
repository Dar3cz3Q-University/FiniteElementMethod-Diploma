#include "ElementMatrixBuilder.h"

#include "math/math.h"
#include "integration/integration.h"
#include "logger/logger.h"
#include "utils/utils.h"

#include <iostream>

namespace fem::domain
{

std::expected<ElementMatrices, int> ElementMatrixBuilder::BuildQuadMatrices(const mesh::model::Mesh& mesh, const mesh::model::Quad& quad) const
{
	constexpr auto schema = integration::IntegrationSchema::Gauss3;

	LOG_TRACE(
		"Assembling element matrices for quad id={} nodes=[{}, {}, {}, {}] using Gauss{}",
		quad.id, quad.nodeIDs[0], quad.nodeIDs[1], quad.nodeIDs[2], quad.nodeIDs[3], std::to_underlying(schema)
	);

	const auto& quadData = integration::GetQuadIntegrationData(schema);

	LOG_TRACE("Quad integration: nGauss1D={}, nPoints={}", quadData.nGauss, quadData.nPoints);

	ElementMatrices out;
	out.H.setZero();
	out.C.setZero();
	out.P.setZero();

	// Precompute material constants
	const double k = m_Material.conductivity;
	const double rhoC = m_Material.density * m_Material.specificHeat;

	// Extract node coordinates
	std::array<double, 4> nodeX{};
	std::array<double, 4> nodeY{};
	for (int i = 0; i < 4; i++)
	{
		const auto& node = mesh.GetNode(quad.nodeIDs[i]);
		nodeX[i] = node.x;
		nodeY[i] = node.y;

		LOG_TRACE("Node local {} (id={}): x={:.6f}, y={:.6f}", i, quad.nodeIDs[i], node.x, node.y);
	}

	const int nPoints = quadData.nPoints;
	for (int i = 0; i < nPoints; i++)
	{
		const auto& dN_dKsi = quadData.dN_dKsi[i];
		const auto& dN_dEta = quadData.dN_dEta[i];
		const auto& N_N_T = quadData.N_N_T[i];
		const double w = quadData.weights[i];

		LOG_TRACE("IP={}: ksi={:.6f}, eta={:.6f}, w={:.6f}", i, quadData.ksi[i], quadData.eta[i], w);

		// Compute Jacobian components directly
		double J00 = 0.0, J01 = 0.0, J10 = 0.0, J11 = 0.0;
		for (int a = 0; a < 4; a++)
		{
			J00 += dN_dKsi[a] * nodeX[a];
			J01 += dN_dEta[a] * nodeX[a];
			J10 += dN_dKsi[a] * nodeY[a];
			J11 += dN_dEta[a] * nodeY[a];
		}

		const double detJ = J00 * J11 - J01 * J10;
		const double invDetJ = 1.0 / detJ;

		// Inverse Jacobian components
		const double invJ00 = J11 * invDetJ;
		const double invJ01 = -J01 * invDetJ;
		const double invJ10 = -J10 * invDetJ;
		const double invJ11 = J00 * invDetJ;

		LOG_TRACE("GP {}: detJ = {:.6e}", i, detJ);

		// Compute gradients in physical coordinates
		std::array<double, 4> dN_dx{};
		std::array<double, 4> dN_dy{};
		for (int a = 0; a < 4; a++)
		{
			dN_dx[a] = invJ00 * dN_dKsi[a] + invJ10 * dN_dEta[a];
			dN_dy[a] = invJ01 * dN_dKsi[a] + invJ11 * dN_dEta[a];

			LOG_TRACE("dN/dx = {}, dN/dy = {}", dN_dx[a], dN_dy[a]);
		}

		// Precompute common factor
		const double detJ_w = detJ * w;
		const double k_detJ_w = k * detJ_w;
		const double rhoC_detJ_w = rhoC * detJ_w;

		// Build H and C matrices
		for (int a = 0; a < 4; a++)
		{
			for (int b = 0; b < 4; b++)
			{
				const double gradDot = dN_dx[a] * dN_dx[b] + dN_dy[a] * dN_dy[b];
				out.H(a, b) += k_detJ_w * gradDot;
				out.C(a, b) += rhoC_detJ_w * N_N_T[a][b];
			}
		}
	}

	return out;
}

std::expected<BoundaryMatrices, int> ElementMatrixBuilder::BuildLineBoundaryMatrices(const mesh::model::Mesh& mesh, const mesh::model::Line& line) const
{
	auto schema = integration::IntegrationSchema::Gauss2;

	LOG_TRACE(
		"Building boundary matrices for line nodes=[{}, {}] using Gauss{}",
		line.nodeIDs[0], line.nodeIDs[1], std::to_underlying(schema)
	);

	BoundaryMatrices out;
	out.H.setZero();
	out.P.setZero();

	const auto& node1 = mesh.GetNode(line.nodeIDs[0]);
	const auto& node2 = mesh.GetNode(line.nodeIDs[1]);

	Vec2 p1(node1.x, node1.y);
	Vec2 p2(node2.x, node2.y);

	LOG_TRACE("Node 1 (id={}): x={:.6f}, y={:.6f}",
		line.nodeIDs[0], node1.x, node1.y);
	LOG_TRACE("Node 2 (id={}): x={:.6f}, y={:.6f}",
		line.nodeIDs[1], node2.x, node2.y);

	double length = (p2 - p1).norm();
	double detJ = length / 2.0;

	LOG_TRACE("Edge length: {:.6f}, detJ: {:.6f}", length, detJ);

	const auto& lineData = integration::GetLineIntegrationData(schema);

	LOG_TRACE("Using integration schema: Gauss{}, nPoints={}",
		std::to_underlying(schema), lineData.nPoints);

	// TODO: Check if values are present
	double alpha = m_BoundaryCondition.alpha.value();
	double ambientTemperature = m_BoundaryCondition.ambientTemperature.value();

	LOG_TRACE("Convection BC: alpha={:.2f} W/(m^2*K), T_ambient={:.2f} K",
		alpha, ambientTemperature);

	for (int i = 0; i < lineData.nPoints; ++i)
	{
		double xi = lineData.ksi[i];
		double w = lineData.weights[i];
		const auto& N = lineData.N[i];
		const auto& N_N_T = lineData.N_N_T[i];

		LOG_TRACE("IP {}: xi={:.6f}, w={:.6f}, N=[{:.6f}, {:.6f}]",
			i, xi, w, N[0], N[1]);

		Mat2 N_N_T_local;
		N_N_T_local << N_N_T[0][0], N_N_T[0][1],
			N_N_T[1][0], N_N_T[1][1];

		out.H += alpha * N_N_T_local * detJ * w;

		Vec2 N_eigen(N[0], N[1]);
		out.P += alpha * ambientTemperature * N_eigen * detJ * w;
	}

	return out;
}

}
