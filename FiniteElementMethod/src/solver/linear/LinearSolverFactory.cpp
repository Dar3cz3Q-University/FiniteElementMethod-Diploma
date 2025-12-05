#include "LinearSolverFactory.h"

#include "LinearSolverType.h"

#include "cholesky/CholeskyLDLTSolver.h"
#include "lu/SparseLUSolver.h"
#include "qr/SparseQRSolver.h"

#include "logger/logger.h"

namespace fem::solver::linear
{

std::unique_ptr<ILinearSolver> LinearSolverFactory::Create(LinearSolverType type)
{
	using enum LinearSolverType;

	switch (type)
	{
	case SimplicialLDLT:
		return std::make_unique<CholeskyLDLTSolver>();

	case SparseLU:
		return std::make_unique<SparseLUSolver>();

	case SparseQR:
		return std::make_unique<SparseQRSolver>();

	default:
		LOG_ERROR("Unknown solver type: {}", LinearSolverTypeToString(type));
		std::unreachable();
		std::abort();
	}
}

} // namespace fem::solver::linear
