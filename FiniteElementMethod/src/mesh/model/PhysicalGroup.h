#pragma once

#include <string>
#include <vector>

namespace fem::mesh::model
{

struct PhysicalGroup
{
	int tag;
	int dimension;
	std::string name;
	std::vector<std::size_t> lineIDs;
};

}
