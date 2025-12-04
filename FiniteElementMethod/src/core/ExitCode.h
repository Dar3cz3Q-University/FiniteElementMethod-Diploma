#pragma once

namespace fem::core
{

/// <summary>
/// Exit codes returned by the FEM application process
/// </summary>
enum class ExitCode : int
{
	/// <summary>
	/// Application finished successfully without errors
	/// </summary>
	Success = 0,

	/// <summary>
	/// Application terminated due to a command-line interface (CLI) error
	/// </summary>
	CliError,

	/// <summary>
	/// Application terminated due to an error that occurs while processing or validating configuration file
	/// </summary>
	ConfigError,

	/// <summary>
	/// Application terminated due to a mesh-related error
	/// </summary>
	MeshError,

	/// <summary>
	/// Application terminated due to a domain setup or material definition error
	/// </summary>
	DomainError,

	/// <summary>
	/// Application terminated due to a solver error
	/// </summary>
	SolverError,
};

} // namespace fem::core
