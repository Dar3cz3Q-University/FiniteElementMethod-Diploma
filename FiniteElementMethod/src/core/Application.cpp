#include "Application.h"

#include "cache/cache.h"
#include "config/config.h"
#include "domain/domain.h"
#include "fileio/fileio.h"
#include "logger/logger.h"
#include "mesh/mesh.h"
#include "solver/solver.h"

#include <filesystem>
#include <iostream>
#include <thread>

#include "gmsh.h"

namespace fem::core
{

namespace fs = std::filesystem;

Application::Application(const ApplicationOptions& options)
	: m_Options(options)
{
}

Application::~Application()
{
	TearDown();
}

void Application::Initialize()
{
	if (m_Initialized)
		return;

	fem::logger::Log::Init(m_Options.logLevel);
	gmsh::initialize();

	size_t nThreads = std::thread::hardware_concurrency();

	if (m_Options.numberOfThreads.has_value())
		nThreads = m_Options.numberOfThreads.value();

	config::OMPConfig::SetNumThreads(nThreads);

	config::MKLConfig::SetDynamic(false);
	config::MKLConfig::SetNumThreads(nThreads);

	LOG_INFO("Initialization completed");
	LOG_INFO(m_Options.ToString());

	m_Initialized = true;
}

void Application::TearDown() noexcept
{
	if (!m_Initialized)
		return;

	LOG_INFO("Application shutting down...");

	gmsh::finalize();

	m_Initialized = false;
}

ExitCode Application::Execute()
{
	using enum ExitCode;

	if (m_Options.showHelp) return Success;

	Initialize();

	LOG_INFO("Application running...");

	config::OMPConfig::PrintInfo();
	config::MKLConfig::PrintInfo();

	const auto& parsedConfig = config::loader::ConfigLoader::LoadFromFile(m_Options.configFilePath);

	if (!parsedConfig)
	{
		LOG_ERROR(parsedConfig.error().ToString());
		return ConfigError;
	}

	const auto& config = *parsedConfig;

	mesh::provider::MeshProvider provider{};
	const auto& meshResult = provider.LoadMesh(config.meshPath);

	if (!meshResult)
	{
		LOG_ERROR(meshResult.error().ToString());
		return MeshError;
	}

	const auto& mesh = *meshResult;

	SpMat H, C;
	Vec P;

	bool cacheHit = false;

	if (m_Options.useCache)
	{
		auto cachedSystem = cache::CacheManager::LoadSystem(cache::CACHE_ROOT, parsedConfig->meshPath.string(), m_Options.configFilePath.string(), true);

		if (cachedSystem)
		{
			H = std::move(cachedSystem->H);
			C = std::move(cachedSystem->C);
			P = std::move(cachedSystem->P);

			LOG_INFO("System loaded from cache");
			cacheHit = true;
		}
	}
	else
	{
		LOG_INFO("Cache disabled");
	}

	if (!cacheHit)
	{
		LOG_INFO("Assembling system...");

		domain::ElementMatrixBuilder elementBuilder(
			config.material,
			config.boundaryCondition // TODO: Use vector of BCs
		);

		domain::GlobalMatrixBuilder matrixBuilder(mesh, elementBuilder);

		const auto& buildResult = matrixBuilder.Build();

		if (!buildResult)
		{
			LOG_ERROR(buildResult.error());
			return DomainError;
		}

		H = buildResult->matrices.H;
		C = buildResult->matrices.C;
		P = buildResult->matrices.P;

		if (m_Options.useCache)
		{
			cache::CacheManager::SaveTransientSystem(cache::CACHE_ROOT, H, C, P, parsedConfig->meshPath.string(), m_Options.configFilePath.string());
		}
	}

	auto solverConfig = solver::FEMSolverConfig{
		.problemType = config.problemType,
		.linearSolver = m_Options.LinearSolverType,
	};

	if (config.problemType == domain::model::ProblemType::Transient)
	{
		solverConfig.transientConfig = config.transientConfig;
	}

	auto solver = solver::FEMSolver();
	auto solution = solver.Solve(H, C, P, solverConfig);

	if (!solution)
	{
		LOG_ERROR(solution.error().ToString());
		return SolverError;
	}

	if (m_Options.metricsFilePath.has_value())
	{
		const auto& metricsExported = fileio::StatsExporter::Export(
			m_Options.metricsFilePath.value(),
			solver::linear::LinearSolverTypeToString(m_Options.LinearSolverType),
			solution->stats);

		if (!metricsExported)
		{
			LOG_ERROR(metricsExported.error());
			return MetricsExportError;
		}
	}

	if (m_Options.useCache)
	{
		cache::CacheManager::PrintCacheInfo(cache::CACHE_ROOT, parsedConfig->meshPath.string(), m_Options.configFilePath.string());
	}

	// TODO: Extract output paths to config
	if (solution->isSteady())
	{
		fs::path vtkPath = "output/solution.vtk";
		fs::create_directories(vtkPath.parent_path());

		auto vtkResult = fileio::VTKExporter::ExportSteady(vtkPath, mesh, solution->getFinalSolution());
		if (!vtkResult)
		{
			LOG_ERROR(vtkResult.error().ToString());
			return VTKExportError;
		}
	}
	else
	{
		const auto& transient = solution->getTransient();
		fs::path outputDir = "output";

		auto vtkResult = fileio::VTKExporter::ExportTransient(
			outputDir,
			mesh,
			transient.temperatures,
			transient.timeSteps);

		if (!vtkResult)
		{
			LOG_ERROR(vtkResult.error().ToString());
			return VTKExportError;
		}
	}

	return Success;
}

} // namespace fem::core
