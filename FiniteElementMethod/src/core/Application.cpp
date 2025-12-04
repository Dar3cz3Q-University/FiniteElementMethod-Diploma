#include "Application.h"

#include "config/config.h"
#include "domain/domain.h"
#include "logger/logger.h"
#include "mesh/mesh.h"

#include <iostream>
#include <thread>

#include "gmsh.h"
#include <omp.h>

namespace fem::core
{

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

	omp_set_num_threads(nThreads);

	LOG_INFO("Initialization completed");

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

	const auto& parsedConfig = config::loader::ConfigLoader::LoadFromFile(m_Options.configFilePath);

	if (!parsedConfig)
	{
		LOG_ERROR(parsedConfig.error().ToString());
		return ConfigError;
	}

	const auto& config = *parsedConfig;

	mesh::provider::MeshProvider provider{};
	const auto& mesh = provider.LoadMesh(config.meshPath);

	if (!mesh)
	{
		LOG_ERROR(mesh.error().ToString());
		return MeshError;
	}

	domain::ElementMatrixBuilder elementBuilder(
		config.material,
		config.boundaryCondition // TODO: Use vector of BCs
	);

	domain::GlobalMatrixBuilder matrixBuilder(*mesh, elementBuilder);

	const auto& matrices = matrixBuilder.Build();

	if (!matrices)
	{
		LOG_ERROR(matrices.error());
		return DomainError;
	}

	// TODO: Run solver

	return Success;
}

} // namespace fem::core
