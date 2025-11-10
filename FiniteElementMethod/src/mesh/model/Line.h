#pragma once

#include <array>
#include <cstddef>

namespace fem::mesh::model
{

struct Line
{
	std::size_t id;
	std::array<std::size_t, 2> nodeIDs;
};

}
