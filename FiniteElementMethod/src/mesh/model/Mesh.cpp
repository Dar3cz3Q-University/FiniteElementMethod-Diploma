#include "Mesh.h"

namespace fem::mesh::model
{

Mesh::Mesh(size_t numberOfNodes, size_t numberOfCells, size_t numberOfLines)
{
	m_Nodes.reserve(numberOfNodes);
	m_Quads.reserve(numberOfCells);
	m_Lines.reserve(numberOfLines);
}

std::optional<PhysicalGroup> Mesh::GetPhysicalGroupByName(const std::string_view& name) const
{
	for (const auto& group : m_PhysicalGroups)
		if (group.name == name)
			return group;

	return std::nullopt;
}

}
