#pragma once

namespace fem::mesh::provider
{

enum class GmshElementType : int
{
	Line2 = 1,
	Triangle3,
	Quad4,
};

}
