#pragma once

#include <string>
#include <format>

namespace fem::domain::integration
{

enum class IntegrationErrorCode : int
{
	IncorrectIntegrationSchema = 0,
	Unknown,
};

struct IntegrationError
{
	IntegrationErrorCode code;
	std::string message;

	std::string ToString() const
	{
		using enum IntegrationErrorCode;

		std::string msg = "IntegrationError: ";

		switch (code)
		{
		case IncorrectIntegrationSchema:
			msg += "Incorrect integration schema.\n";
			break;
		case Unknown:
			msg += "Unknown error.\n";
			break;
		}

		if (!message.empty())
		{
			msg += "(";
			msg += message;
			msg += ")";
		}

		return msg;
	}
};

} // namespace fem::domain::integration

template <>
struct std::formatter<fem::domain::integration::IntegrationError> : std::formatter<std::string_view>
{
	auto format(const fem::domain::integration::IntegrationError& err, std::format_context& ctx) const
	{
		return std::formatter<std::string_view>::format(err.ToString(), ctx);
	}
};
