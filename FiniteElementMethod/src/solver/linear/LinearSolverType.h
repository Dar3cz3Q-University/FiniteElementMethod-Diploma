#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <format>

namespace fem::solver::linear
{

enum class LinearSolverType : int
{
	SimplicialLDLT = 0,
	SparseLU,
	SparseQR,
};

struct LinearSolverInfo
{
	LinearSolverType type;
	std::string name;
	std::string description;
};

inline static const std::array<LinearSolverInfo, 3> LINEAR_SOLVERS = { {
	{ LinearSolverType::SimplicialLDLT, "cholesky", "Cholesky decomposition (fastest for SPD)" },
	{ LinearSolverType::SparseLU,       "lu",       "LU decomposition (general purpose)" },
	{ LinearSolverType::SparseQR,       "qr",       "QR decomposition (most stable)" }
} };

inline std::optional<LinearSolverType> ParseSolverType(std::string_view str)
{
	using enum LinearSolverType;

	if (str == "cholesky") return SimplicialLDLT;
	else if (str == "lu") return SparseLU;
	else if (str == "qr") return SparseQR;

	return std::nullopt;
}

inline std::string_view LinearSolverTypeToString(LinearSolverType type)
{
	for (const auto& info : LINEAR_SOLVERS)
		if (info.type == type)
			return info.name;

	std::unreachable();
}

} // namespace fem::solver::linear
