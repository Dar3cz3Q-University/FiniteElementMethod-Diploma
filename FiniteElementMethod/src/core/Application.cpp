#include "Application.h"

#include "logger/Log.h"

namespace fem::core {

Application::Application()
{
	Initialize();
	Execute();
}

Application::~Application()
{
	LOG_INFO("Application shutting down...");
}

void Application::Initialize()
{
	fem::logger::Log::Init();

	LOG_INFO("Initialization completed");
}

void Application::Execute()
{
	LOG_INFO("Application running...");
}

}
