#pragma once

#include <string>

namespace fem::domain::model
{

struct Material
{
	std::string name;
	double conductivity;
	double density;
	double specificHeat;
};

} // namespace fem::domain::model
