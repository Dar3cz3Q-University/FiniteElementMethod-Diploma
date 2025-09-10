#pragma once

namespace fem::core {

class Application
{
public:
	Application();
	~Application();

private:
	void Initialize();
	void Execute();
};

}
