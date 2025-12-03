#include "IntegrationData.h"

#include "assertion/assertion.h"

namespace fem::domain::integration
{

const QuadIntegrationData& GetQuadIntegrationData(IntegrationSchema schema)
{
	using enum IntegrationSchema;

	auto initializeData = [](IntegrationSchema s)
		{
			auto result = BuildQuadIntegrationData(s);
			if (!result)
			{
				LOG_ERROR("Failed to build quad integration data: {}", result.error().message);
				std::abort();
			}
			return std::move(result.value());
		};

	switch (schema)
	{
	case Gauss1: {
		static QuadIntegrationData quad1x1 = initializeData(Gauss1);
		return quad1x1;
	}
	case Gauss2: {
		static QuadIntegrationData quad2x2 = initializeData(Gauss2);
		return quad2x2;
	}
	case Gauss3: {
		static QuadIntegrationData quad3x3 = initializeData(Gauss3);
		return quad3x3;
	}
	case Gauss4: {
		static QuadIntegrationData quad4x4 = initializeData(Gauss4);
		return quad4x4;
	}
	case Gauss5: {
		static QuadIntegrationData quad5x5 = initializeData(Gauss5);
		return quad5x5;
	}
	default: {
		LOG_ERROR("Unsupported integration schema: {}", std::to_underlying(schema));
		std::abort();
		std::unreachable();
	}
	}
}

const LineIntegrationData& GetLineIntegrationData(IntegrationSchema schema)
{
	using enum IntegrationSchema;

	auto initializeData = [](IntegrationSchema s)
		{
			auto result = BuildLineIntegrationData(s);
			if (!result)
			{
				LOG_ERROR("Failed to build line integration data: {}", result.error().message);
				std::abort();
			}
			return std::move(result.value());
		};

	switch (schema)
	{
	case Gauss1: {
		static LineIntegrationData line1x1 = initializeData(Gauss1);
		return line1x1;
	}
	case Gauss2: {
		static LineIntegrationData line2x2 = initializeData(Gauss2);
		return line2x2;
	}
	case Gauss3: {
		static LineIntegrationData line3x3 = initializeData(Gauss3);
		return line3x3;
	}
	case Gauss4: {
		static LineIntegrationData line4x4 = initializeData(Gauss4);
		return line4x4;
	}
	case Gauss5: {
		static LineIntegrationData line5x5 = initializeData(Gauss5);
		return line5x5;
	}
	default: {
		LOG_ERROR("Unsupported integration schema: {}", std::to_underlying(schema));
		std::abort();
		std::unreachable();
	}
	}
}

}
