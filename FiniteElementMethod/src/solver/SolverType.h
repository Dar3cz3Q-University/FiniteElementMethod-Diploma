#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <format>

namespace fem::solver
{

enum class SolverType : int
{
	SimplicialLDLT = 0,
	SparseLU,
	SparseQR,
};

struct SolverInfo
{
	SolverType type;
	std::string name;
	std::string description;
};

inline std::optional<SolverType> ParseSolverType(std::string_view str)
{
	using enum SolverType;

	if (str == "cholesky") return SimplicialLDLT;
	else if (str == "lu") return SparseLU;
	else if (str == "qr") return SparseQR;

	return std::nullopt;
}

inline static const std::array<SolverInfo, 3> LINEAR_SOLVERS = { {
	{ SolverType::SimplicialLDLT, "cholesky", "Cholesky decomposition (fastest for SPD)" },
	{ SolverType::SparseLU,       "lu",       "LU decomposition (general purpose)" },
	{ SolverType::SparseQR,       "qr",       "QR decomposition (most stable)" }
} };

} // namespace fem::solver
