#pragma once

#include <string>

namespace fem::domain
{

struct Material
{
    std::string name;
    double conductivity;
    double alpha;
    double density;
    double specificHeat;
};

}
