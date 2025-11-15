#pragma once

#include "ApplicationOptions.h"
#include "ExitCode.h"

namespace fem::core
{

/// <summary>
/// Main entry point for running the FEM application logic.
/// Responsible for initializing subsystems, executing the simulation
/// and performing cleanup.
/// </summary>
class Application
{
public:
    /// <summary>
    /// Constructs the application with the given configuration options.
    /// </summary>
    /// <param name="options">Configuration options for the FEM application.</param>
    explicit Application(const ApplicationOptions& options);

    /// <summary>
    /// Destroys the application instance and performs any remaining cleanup.
    /// </summary>
    ~Application() noexcept;

    /// <summary>
    /// Executes the FEM application workflow:
    /// initialization, problem setup, solving and finalization.
    /// </summary>
    /// <returns>
    /// An <see cref="ExitCode"/> indicating whether the execution
    /// completed successfully or failed with an error.
    /// </returns>
    ExitCode Execute();

private:
    /// <summary>
    /// Indicates whether the application has been successfully initialized.
    /// </summary>
    bool m_Initialized = false;

    /// <summary>
    /// Configuration options used by the application during execution.
    /// </summary>
    ApplicationOptions m_Options;

    /// <summary>
    /// Initializes the FEM application, including resources, mesh, domain
    /// and solver setup.
    /// </summary>
    void Initialize();

    /// <summary>
    /// Releases resources and performs application teardown.
    /// Guaranteed not to throw.
    /// </summary>
    void TearDown() noexcept;
};

} // namespace fem::core
