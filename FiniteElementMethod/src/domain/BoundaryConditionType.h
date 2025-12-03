#pragma once

namespace fem::domain
{

/// <summary>
/// Types of boundary conditions supported by the FEM solver
/// for boundary integrations and matrix assembly.
/// </summary>
enum class BoundaryConditionType : int
{
    /// <summary>
    /// Convection boundary condition.
    /// </summary>
    Convection
};

}
