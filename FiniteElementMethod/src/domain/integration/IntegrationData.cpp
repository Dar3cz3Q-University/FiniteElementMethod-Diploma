#include "IntegrationData.h"

namespace fem::domain::integration
{

const QuadIntegrationData& GetQuadIntegrationData(IntegrationSchema schema)
{
	using enum IntegrationSchema;

	switch (schema)
	{
	case Gauss1: {
		static QuadIntegrationData quad1x1 = BuildQuadIntegrationData(Gauss1);
		return quad1x1;
	}
	case Gauss2: {
		static QuadIntegrationData quad2x2 = BuildQuadIntegrationData(Gauss2);
		return quad2x2;
	}
	case Gauss3: {
		static QuadIntegrationData quad3x3 = BuildQuadIntegrationData(Gauss3);
		return quad3x3;
	}
	case Gauss4: {
		static QuadIntegrationData quad4x4 = BuildQuadIntegrationData(Gauss4);
		return quad4x4;
	}
	case Gauss5: {
		static QuadIntegrationData quad5x5 = BuildQuadIntegrationData(Gauss5);
		return quad5x5;
	}
	default: return QuadIntegrationData{}; // TODO: Return error
	}
}

const LineIntegrationData& GetLineIntegrationData(IntegrationSchema schema)
{
	using enum IntegrationSchema;

	switch (schema)
	{
	case Gauss1: {
		static LineIntegrationData line1x1 = BuildLineIntegrationData(Gauss1);
		return line1x1;
	}
	case Gauss2: {
		static LineIntegrationData line2x2 = BuildLineIntegrationData(Gauss2);
		return line2x2;
	}
	case Gauss3: {
		static LineIntegrationData line3x3 = BuildLineIntegrationData(Gauss3);
		return line3x3;
	}
	case Gauss4: {
		static LineIntegrationData line4x4 = BuildLineIntegrationData(Gauss4);
		return line4x4;
	}
	case Gauss5: {
		static LineIntegrationData line5x5 = BuildLineIntegrationData(Gauss5);
		return line5x5;
	}
	default: return LineIntegrationData{}; // TODO: Return error
	}
}

}
