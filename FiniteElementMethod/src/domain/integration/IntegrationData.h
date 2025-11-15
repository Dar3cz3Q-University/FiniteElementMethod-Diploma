#pragma once

#include "IntegrationSchema.h"
#include "QuadIntegrationData.h"

namespace fem::domain::integration
{

const QuadIntegrationData& GetQuadIntegrationData(IntegrationSchema schema);

}
