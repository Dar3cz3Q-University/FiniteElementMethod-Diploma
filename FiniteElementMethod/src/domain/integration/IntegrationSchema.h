#pragma once

namespace fem::domain::integration
{

/// <summary>
/// Available Gauss–Legendre integration schemes (number of quadrature points)
/// used for numerical integration over the reference interval [-1, 1].
/// </summary>
enum class IntegrationSchema : int
{
    /// <summary>
    /// 1-point Gauss–Legendre integration scheme.
    /// </summary>
    Gauss1 = 1,

    /// <summary>
    /// 2-point Gauss–Legendre integration scheme.
    /// </summary>
    Gauss2 = 2,

    /// <summary>
    /// 3-point Gauss–Legendre integration scheme.
    /// </summary>
    Gauss3 = 3,

    /// <summary>
    /// 4-point Gauss–Legendre integration scheme.
    /// </summary>
    Gauss4 = 4,

    /// <summary>
    /// 5-point Gauss–Legendre integration scheme.
    /// </summary>
    Gauss5 = 5,
};

}
