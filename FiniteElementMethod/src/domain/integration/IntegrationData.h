#pragma once

#include "IntegrationSchema.h"
#include "quad/QuadIntegrationData.h"
#include "line/LineIntegrationData.h"

namespace fem::domain::integration
{

const QuadIntegrationData& GetQuadIntegrationData(IntegrationSchema schema);
const LineIntegrationData& GetLineIntegrationData(IntegrationSchema schema);

}
