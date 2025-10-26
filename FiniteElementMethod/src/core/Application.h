#pragma once

#include "ApplicationOptions.h"
#include "ExitCode.h"

namespace fem::core {

class Application
{
public:
	Application(const ApplicationOptions& options);
	~Application();

	ExitCode Execute();

private:
	bool m_Initialized = false;
	ApplicationOptions m_Options;

	void Initialize();
	void TearDown();
};

}
