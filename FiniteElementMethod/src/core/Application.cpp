#include "Application.h"

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

	LOG_INFO("Initialization completed");

	m_Initialized = true;
}

void Application::TearDown()
{
	if (!m_Initialized)
		return;

	LOG_INFO("Application shutting down...");

	m_Initialized = false;
}

ExitCode Application::Execute()
{
	if (m_Options.ShowHelp) return ExitCode::Success;

	Initialize();

	LOG_INFO("Application running...");
	return ExitCode::Success;
}

}
