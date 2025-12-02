#include "Application.h"

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

	fem::logger::Log::Init();
	gmsh::initialize();

	size_t nThreads = std::thread::hardware_concurrency();

	if (m_Options.NumberOfThreads.has_value())
	{
		nThreads = m_Options.NumberOfThreads.value();
	}

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
	if (m_Options.ShowHelp) return ExitCode::Success;

	Initialize();

	LOG_INFO("Application running...");

	mesh::provider::MeshProvider provider{};

	// TODO: Remove const path
	m_Options.MeshInputPath = "D:\\Studia\\Praca inzynierska\\FiniteElementMethod-Diploma\\assets\\mesh\\test.msh";

	const auto& result = provider.LoadMesh(m_Options.MeshInputPath);

	if (!result)
	{
		std::cout << result.error().message << "\n";
		//LOG_ERROR(result.error());
		return ExitCode::MeshError;
	}

	domain::ElementMatrixBuilder elementBuilder(
		domain::Material("steel", 25, 7800, 700),
		domain::BoundaryCondition("test", domain::BoundaryConditionType::Convection, 1200, 300));

	domain::GlobalMatrixBuilder matrixBuilder(*result, elementBuilder);

	const auto& matrices = matrixBuilder.Build();

	return ExitCode::Success;
}

}
