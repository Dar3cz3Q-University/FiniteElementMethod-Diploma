#pragma once

#include "ApplicationOptions.h"
#include "ExitCode.h"

#include "domain/GlobalMatrixBuilder.h"

#include "mesh/model/Mesh.h"
#include "mesh/provider/MeshProvider.h"

#include "gmsh.h"
#include "logger/Log.h"

namespace fem::core {

class Application
{
public:
	explicit Application(const ApplicationOptions& options);
	~Application() noexcept;

	ExitCode Execute();

private:
	bool m_Initialized = false;
	ApplicationOptions m_Options;

	void Initialize();
	void TearDown() noexcept;
};

}
