#pragma once

#include "IntegrationSchema.h"
#include "quad/QuadIntegrationData.h"
#include "line/LineIntegrationData.h"

namespace fem::domain::integration
{

/// <summary>
/// Retrieves precomputed Gauss-Legendre integration data for a 4-node quadrilateral
/// element corresponding to the specified integration scheme.
/// </summary>
/// <param name="schema">
/// Integration scheme determining the number of Gauss points per direction.
/// </param>
/// <returns>
/// A const reference to a cached <see cref="QuadIntegrationData"/> instance
/// associated with the given integration scheme.
/// </returns>
const QuadIntegrationData& GetQuadIntegrationData(IntegrationSchema schema);

/// <summary>
/// Retrieves precomputed Gauss-Legendre integration data for a 2-node line element
/// corresponding to the selected integration scheme.
/// </summary>
/// <param name="schema">
/// Integration scheme specifying the number of Gauss points along the line.
/// </param>
/// <returns>
/// A const reference to a cached <see cref="LineIntegrationData"/> instance
/// associated with the specified integration scheme.
/// </returns>
const LineIntegrationData& GetLineIntegrationData(IntegrationSchema schema);

}
