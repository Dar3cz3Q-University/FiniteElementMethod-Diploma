#pragma once

namespace fem::core
{

enum class ExitCode : int
{
	Success = 0,
	CliError,
	ConfigError,
	MeshError,
	DomainError,
	SolverError,
};

} // namespace fem::core
