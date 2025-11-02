#include "Mesh.h"

namespace fem::mesh::model
{

Mesh::Mesh(size_t numberOfNodes, size_t numberOfCells, size_t numberOfLines)
{
	m_Nodes.reserve(numberOfNodes);
	m_Quads.reserve(numberOfCells);
	m_Lines.reserve(numberOfLines);
}

}
