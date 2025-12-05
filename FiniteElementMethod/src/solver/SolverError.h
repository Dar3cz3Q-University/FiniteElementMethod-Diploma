#pragma once

#include <string>

namespace fem::solver
{

enum class SolverErrorCode : int
{
	SingularMatrix = 0,
	NumericalInstability,
	InvalidInput,
	Unknown,
};

struct SolverError
{
	SolverErrorCode code;
	std::string message;

	std::string ToString() const
	{
		std::string msg = "SolverError: ";
		switch (code)
		{
		case SolverErrorCode::SingularMatrix:
			msg += "Singular matrix";
			break;
		case SolverErrorCode::NumericalInstability:
			msg += "Numerical instability";
			break;
		case SolverErrorCode::InvalidInput:
			msg += "Invalid input";
			break;
		case SolverErrorCode::Unknown:
			msg += "Unknown error";
			break;
		}

		if (!message.empty())
		{
			msg += " (";
			msg += message;
			msg += ")";
		}

		return msg;
	}
};

} // namespace fem::solver
