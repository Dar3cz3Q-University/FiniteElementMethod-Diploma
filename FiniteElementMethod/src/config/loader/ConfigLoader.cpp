#include "ConfigLoader.h"

#include "domain/domain.h"
#include "fileio/fileio.h"

#include <format>
#include <fstream>

#include <nlohmann/json.hpp>

namespace fem::config::loader
{

std::expected<ProblemConfig, ConfigLoaderError> ConfigLoader::LoadFromFile(const std::filesystem::path& configPath)
{
	auto fileService = fileio::FileService();
	const auto& readData = fileService.Read(configPath);

	if (!readData)
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::FileError,
				std::format("Failed to read config file: {}", readData.error())
			}
		);

	return Parse(*readData);
}

std::expected<ProblemConfig, ConfigLoaderError> ConfigLoader::Parse(const std::string& jsonContent)
{
	nlohmann::json json;

	try
	{
		json = nlohmann::json::parse(jsonContent);
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::ParserError,
				std::format("JSON parsing failed: {}", e.what())
			}
		);
	}

	ProblemConfig config{};

	try
	{
		if (auto res = ExtractMesh(json, &config); !res)
			return std::unexpected(res.error());

		if (auto res = ExtractProblemType(json, &config); !res)
			return std::unexpected(res.error());

		if (config.problemType == domain::model::ProblemType::Transient)
			if (auto res = ExtractTransientParams(json, &config); !res)
				return std::unexpected(res.error());

		if (auto res = ExtractMaterial(json, &config); !res)
			return std::unexpected(res.error());

		if (auto res = ExtractBoundaryCondition(json, &config); !res)
			return std::unexpected(res.error());
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::ParserError,
				std::format("Error parsing config: {}", e.what())
			}
		);
	}
	catch (const std::exception& e)
	{
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::Unknown,
				std::format("Unexpected error: {}", e.what())
			}
		);
	}

	return config;
}

std::expected<void, ConfigLoaderError> ConfigLoader::ExtractMesh(const nlohmann::json& json, ProblemConfig* config)
{
	auto meshPath = GetRequiredField<std::string>(json, "/mesh_path");
	if (!meshPath)
		return std::unexpected(meshPath.error());

	config->meshPath = *meshPath;

	return {};
}

std::expected<void, ConfigLoaderError> ConfigLoader::ExtractProblemType(const nlohmann::json& json, ProblemConfig* config)
{
	auto problemTypeStr = GetRequiredField<std::string>(json, "/problem/type");
	if (!problemTypeStr)
		return std::unexpected(problemTypeStr.error());

	auto problemType = domain::model::ParseProblemType(*problemTypeStr);
	if (!problemType)
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::InvalidValue,
				std::format("Invalid problem type: {}", *problemTypeStr)
			}
		);

	config->problemType = *problemType;

	return {};
}

std::expected<void, ConfigLoaderError> ConfigLoader::ExtractTransientParams(const nlohmann::json& json, ProblemConfig* config)
{
	auto totalTime = GetRequiredField<double>(json, "/problem/total_time");
	if (!totalTime)
		return std::unexpected(totalTime.error());

	auto timeStep = GetRequiredField<double>(json, "/problem/time_step");
	if (!timeStep)
		return std::unexpected(timeStep.error());

	auto saveHistory = GetRequiredField<bool>(json, "/problem/save_history");
	if (!saveHistory)
		return std::unexpected(saveHistory.error());

	auto initialTemperature = GetRequiredField<double>(json, "/problem/initial_conditions/uniform_temperature");
	if (!initialTemperature)
		return std::unexpected(initialTemperature.error());

	auto totalTimeValue = *totalTime;
	auto timeStepValue = *timeStep;
	auto saveHistoryValue = *saveHistory;
	auto initialTemperatureValue = *initialTemperature;

	// TODO: Create validator
	if (totalTimeValue <= 0.0 || timeStepValue <= 0.0)
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::InvalidValue,
				"Time parameters must be positive"
			}
		);

	std::optional<size_t> saveStrideOpt = std::nullopt;
	if (saveHistoryValue)
	{
		auto saveStride = GetRequiredField<size_t>(json, "/problem/save_stride");
		if (!saveStride)
			return std::unexpected(saveStride.error());

		auto saveStrideValue = *saveStride;

		if (saveStrideValue == 0)
			return std::unexpected(
				ConfigLoaderError{
					ConfigLoaderErrorCode::InvalidValue,
					"Stride value must be positive"
				}
			);

		saveStrideOpt = saveStrideValue;
	}

	config->transientConfig = domain::model::TransientConfig{
		.totalTime = totalTimeValue,
		.timeStep = timeStepValue,
		.saveHistory = saveHistoryValue,
		.saveStride = saveStrideOpt,
		.initialTemperature = initialTemperatureValue,
	};

	return {};
}

std::expected<void, ConfigLoaderError> ConfigLoader::ExtractMaterial(const nlohmann::json& json, ProblemConfig* config)
{
	auto name = GetRequiredField<std::string>(json, "/material/name");
	if (!name)
		return std::unexpected(name.error());

	auto conductivity = GetRequiredField<double>(json, "/material/conductivity");
	if (!conductivity)
		return std::unexpected(conductivity.error());

	auto density = GetRequiredField<double>(json, "/material/density");
	if (!density)
		return std::unexpected(density.error());

	auto specificHeat = GetRequiredField<double>(json, "/material/specific_heat");
	if (!specificHeat)
		return std::unexpected(specificHeat.error());

	auto conductivityValue = *conductivity;
	auto densityValue = *density;
	auto specificHeatValue = *specificHeat;

	// TODO: Create validator
	if (conductivityValue <= 0.0 || densityValue <= 0.0 || specificHeatValue <= 0.0)
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::InvalidValue,
				"Material properties must be positive"
			}
		);

	config->material = domain::model::Material{
		.name = *name,
		.conductivity = conductivityValue,
		.density = densityValue,
		.specificHeat = specificHeatValue
	};

	return {};
}

std::expected<void, ConfigLoaderError> ConfigLoader::ExtractBoundaryCondition(const nlohmann::json& json, ProblemConfig* config)
{
	// TODO: Use vector of BCs
	auto physicalGroupName = GetRequiredField<std::string>(json, "/boundary_condition/physical_group_name");
	if (!physicalGroupName)
		return std::unexpected(physicalGroupName.error());

	auto typeStr = GetRequiredField<std::string>(json, "/boundary_condition/type");
	if (!typeStr)
		return std::unexpected(typeStr.error());

	auto type = domain::model::ParseBoundaryConditionType(*typeStr);
	if (!type)
		return std::unexpected(
			ConfigLoaderError{
				ConfigLoaderErrorCode::InvalidValue,
				std::format("Invalid boundary condition type: {}", *typeStr)
			}
		);

	auto typeValue = *type;

	auto bc = domain::model::BoundaryCondition{
		.physicalGroupName = *physicalGroupName,
		.type = typeValue,
	};

	if (typeValue == domain::model::BoundaryConditionType::Temperature)
	{
		auto temperature = GetRequiredField<double>(json, "/boundary_condition/temperature");
		if (!temperature)
			return std::unexpected(temperature.error());

		bc.temperature = *temperature;
	}
	else if (typeValue == domain::model::BoundaryConditionType::Flux)
	{
		auto heatFlux = GetRequiredField<double>(json, "/boundary_condition/heat_flux");
		if (!heatFlux)
			return std::unexpected(heatFlux.error());

		bc.heatFlux = *heatFlux;
	}
	else if (typeValue == domain::model::BoundaryConditionType::Convection)
	{
		auto alpha = GetRequiredField<double>(json, "/boundary_condition/alpha");
		if (!alpha)
			return std::unexpected(alpha.error());

		auto ambientTemperature = GetRequiredField<double>(json, "/boundary_condition/ambient_temperature");
		if (!ambientTemperature)
			return std::unexpected(ambientTemperature.error());

		bc.alpha = *alpha;
		bc.ambientTemperature = *ambientTemperature;
	}

	// TODO: Create validator

	config->boundaryCondition = bc;

	return {};
}

} // namespace fem::config
