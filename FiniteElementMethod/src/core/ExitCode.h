#pragma once

namespace fem::core
{

/// <summary>
/// Exit codes returned by the FEM application process.
/// </summary>
enum class ExitCode : int
{
    /// <summary>
    /// Application finished successfully without errors.
    /// </summary>
    Success = 0,

    /// <summary>
    /// Application terminated due to a command-line interface (CLI) error
    /// (e.g. invalid arguments, parsing failure).
    /// </summary>
    CliError,

    /// <summary>
    /// Application terminated due to a mesh-related error
    /// (e.g. invalid mesh, failed mesh loading).
    /// </summary>
    MeshError,

    /// <summary>
    /// Application terminated due to a domain setup or material definition error.
    /// </summary>
    DomainError,

    /// <summary>
    /// Application terminated due to a solver error
    /// (e.g. non-convergence, numerical failure).
    /// </summary>
    SolverError,
};

} // namespace fem::core
