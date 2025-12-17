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
	MetricsExportError,
	VTKExportError,
	SolverError,
};

} // namespace fem::core
