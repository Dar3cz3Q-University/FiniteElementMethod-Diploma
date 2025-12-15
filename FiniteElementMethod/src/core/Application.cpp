#include "Application.h"

#include "config/config.h"
#include "domain/domain.h"
#include "fileio/fileio.h"
#include "logger/logger.h"
#include "mesh/mesh.h"
#include "solver/solver.h"

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
	LOG_INFO("Using {} threads", omp_get_max_threads());

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

	const auto& H = matrices.value().H;
	const auto& C = matrices.value().C;
	const auto& P = matrices.value().P;

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

	// TODO: Export solution to .vtk files

	return Success;
}

} // namespace fem::core
