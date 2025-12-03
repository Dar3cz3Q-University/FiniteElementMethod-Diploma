#include "ElementMatrixBuilder.h"

#include "math/math.h"
#include "integration/integration.h"
#include "logger/logger.h"

#include <iostream>

namespace fem::domain
{

std::expected<ElementMatrices, int> ElementMatrixBuilder::BuildQuadMatrices(const mesh::model::Mesh& mesh, const mesh::model::Quad& quad) const
{
	auto schema = integration::IntegrationSchema::Gauss3;

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

	// TODO: Create mapper from Node to Vec2
	std::array<Vec2, 4> nodes{};
	{
		std::size_t i = 0;
		for (const auto& nodeID : quad.nodeIDs)
		{
			const auto& node = mesh.GetNode(nodeID);
			nodes[i++] = Vec2(node.x, node.y);

			LOG_TRACE(
				"Node local {} (id={}): x={:.6f}, y={:.6f}",
				i, nodeID, node.x, node.y
			);
		}
	}

	for (int i = 0; i < quadData.nPoints; i++)
	{
		const auto& dN_dKsi = quadData.dN_dKsi.at(i);
		const auto& dN_dEta = quadData.dN_dEta.at(i);
		const auto ksi = quadData.ksi.at(i);
		const auto eta = quadData.eta.at(i);
		const auto& N = quadData.N.at(i);
		const auto& w = quadData.weights.at(i);

		LOG_TRACE(
			"IP={}: ksi={:.6f}, eta={:.6f}, w={:.6f}",
			i, ksi, eta, w
		);

		Mat2 J = Mat2::Zero();

		// Jacobian
		for (std::size_t a = 0; a < nodes.size(); a++)
		{
			J(0, 0) += dN_dKsi.at(a) * nodes.at(a).x();
			J(0, 1) += dN_dEta.at(a) * nodes.at(a).x();
			J(1, 0) += dN_dKsi.at(a) * nodes.at(a).y();
			J(1, 1) += dN_dEta.at(a) * nodes.at(a).y();
		}

		double detJ = J.determinant();
		Mat2 invJ = J.inverse();

		LOG_TRACE("GP {}: J =\n{}", i, EigenToString(J));
		LOG_TRACE("GP {}: invJ =\n{}", i, EigenToString(invJ));
		LOG_TRACE("GP {}: detJ = {:.6e}", i, detJ);

		// Derivatives
		std::array<Vec2, 4> gradN{};
		for (std::size_t a = 0; a < nodes.size(); a++)
		{
			Vec2 gradN_local(dN_dKsi.at(a), dN_dEta.at(a));
			gradN[a] = invJ.transpose() * gradN_local;

			LOG_TRACE("dN/dx = {}, dN/dy = {}", gradN[a].x(), gradN[a].y());
		}

		// Building H & C matrix
		for (std::size_t a = 0; a < nodes.size(); a++)
		{
			for (std::size_t b = 0; b < nodes.size(); b++)
			{
				double gradDot = gradN[a].dot(gradN[b]);
				out.H(a, b) += m_Material.conductivity * gradDot * detJ * w;

				double Na = N[a];
				double Nb = N[b];
				out.C(a, b) += m_Material.density * m_Material.specificHeat * Na * Nb * detJ * w;
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

	double alpha = m_BoundaryCondition.alpha;
	double ambientTemperature = m_BoundaryCondition.ambientTemperature;

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
