#pragma once

#include <filesystem>
#include <optional>

namespace fem::core
{

/// <summary>
/// Represents configuration options for the FEM application,
/// typically populated from command-line arguments.
/// </summary>
struct ApplicationOptions
{
    /// <summary>
    /// Indicates whether the application should display the help page and exit.
    /// </summary>
    bool ShowHelp = false;

    /// <summary>
    /// Path to the input file containing mesh.
    /// May be empty if not provided.
    /// </summary>
    std::filesystem::path InputPath;
};

} // namespace fem::core
