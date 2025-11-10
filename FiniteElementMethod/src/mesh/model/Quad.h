#pragma once

#include <array>
#include <cstddef>

namespace fem::mesh::model
{

struct Quad
{
	std::size_t id;
	std::array<std::size_t, 4> nodeIDs;
};

}
