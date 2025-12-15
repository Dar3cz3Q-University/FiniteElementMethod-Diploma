#pragma once

#include "ApplicationOptions.h"
#include "ExitCode.h"

namespace fem::core
{

class Application
{
public:
	explicit Application(const ApplicationOptions& options);
	~Application() noexcept;

	ExitCode Execute();

private:
	void Initialize();
	void TearDown() noexcept;

private:
	bool m_Initialized = false;
	ApplicationOptions m_Options;
};

} // namespace fem::core
