#pragma once

#include <cstddef>

namespace fem::mesh::model
{

struct Node
{
	std::size_t id;
	union
	{
		struct
		{
			double x, y, z;
		};
		struct
		{
			double ksi, eta, zeta;
		};
	};
};

}
