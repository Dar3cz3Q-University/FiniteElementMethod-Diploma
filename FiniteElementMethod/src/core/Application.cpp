#include "Application.h"

#include "domain/domain.h"
#include "logger/logger.h"
#include "mesh/mesh.h"

#include "gmsh.h"

namespace fem::core {

Application::Application(const ApplicationOptions& options)
	: m_Options(options)
{}

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
	domain::ElementMatrixBuilder elementBuilder(domain::Material{});

	// TODO: Remove const path
	m_Options.InputPath = "D:\\Studia\\Praca inzynierska\\FiniteElementMethod-Diploma\\assets\\geo\\simple.msh";

	auto result = provider.LoadMesh(m_Options.InputPath);
		//.transform([](const auto& data)
		//{
		//		return data;
		//})
		//.or_else([](const auto& error)
		//{
		//	//LOG_ERROR("Error while reading mesh file.");
		//});

	return ExitCode::Success;
}

}
