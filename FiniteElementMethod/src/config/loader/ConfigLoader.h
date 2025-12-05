#pragma once

#include "ConfigLoaderError.h"
#include "../ProblemConfig.h"

#include "domain/domain.h"

#include <expected>
#include <filesystem>

#include "nlohmann/json.hpp"

namespace fem::config::loader
{

class ConfigLoader
{
public:
	/// <summary>
	/// Load and parse configuration from JSON file.
	/// Validates all required fields and returns detailed errors on failure.
	/// </summary>
	/// <param name="configPath">Path to JSON configuration file</param>
	/// <returns>Parsed configuration or error</returns>
	static std::expected<ProblemConfig, ConfigLoaderError> LoadFromFile(const std::filesystem::path& configPath);

private:
	static std::expected<ProblemConfig, ConfigLoaderError> Parse(const std::string& jsonContent);

	static std::expected<void, ConfigLoaderError> ExtractMesh(const nlohmann::json& json, ProblemConfig* config);
	static std::expected<void, ConfigLoaderError> ExtractProblemType(const nlohmann::json& json, ProblemConfig* config);
	static std::expected<void, ConfigLoaderError> ExtractTransientParams(const nlohmann::json& json, ProblemConfig* config);
	static std::expected<void, ConfigLoaderError> ExtractMaterial(const nlohmann::json& json, ProblemConfig* config);
	static std::expected<void, ConfigLoaderError> ExtractBoundaryCondition(const nlohmann::json& json, ProblemConfig* config);
	static std::expected<void, ConfigLoaderError> ExtractInitialConditions(const nlohmann::json& json, ProblemConfig* config);

private:
	template<typename T>
	static std::expected<T, ConfigLoaderError> GetRequiredField(const nlohmann::json& json, const std::string& path)
	{
		try
		{
			const auto& value = json.at(nlohmann::json::json_pointer(path));
			return value.get<T>();
		}
		catch (const nlohmann::json::out_of_range& e)
		{
			return std::unexpected(
				ConfigLoaderError{
					ConfigLoaderErrorCode::MissingRequiredField,
					std::format("Missing required field: '{}'", path)
				}
			);
		}
		catch (const nlohmann::json::exception& e)
		{
			return std::unexpected(
				ConfigLoaderError{
					ConfigLoaderErrorCode::InvalidValue,
					std::format("Invalid value for field '{}': {}", path, e.what())
				}
			);
		}
	}

};
} // namespace fem::config::loader
