#include "Application.h"

#include "mesh/model/Mesh.h"
#include "mesh/provider/MeshProvider.h"

#include "gmsh.h"
#include "logger/Log.h"

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

	gmsh::finalize();

	LOG_INFO("Application shutting down...");

	m_Initialized = false;
}

ExitCode Application::Execute()
{
	if (m_Options.ShowHelp) return ExitCode::Success;

	Initialize();

	LOG_INFO("Application running...");

	mesh::provider::MeshProvider provider{};
	auto mesh = provider.LoadMesh("D:\\Studia\\Praca inzynierska\\FiniteElementMethod-Diploma\\assets\\geo\\simple.msh"); // TODO: Get path from options

	return ExitCode::Success;
}

}
