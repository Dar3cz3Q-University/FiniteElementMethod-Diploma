#pragma once

namespace fem::core
{

enum class ExitCode : int
{
	Success = 0,
	CliError,
	MeshError,
	DomainError,
	SolverError,
};

}
