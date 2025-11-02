#pragma once

#include "Node.h"
#include "Line.h"
#include "Quad.h"

#include <vector>

namespace fem::mesh::model
{

class Mesh
{
public:
	Mesh() = default;
	Mesh(size_t numberOfNodes, size_t numberOfCells, size_t numberOfLines);

	inline void AddNode(const Node& node) {
		m_Nodes.push_back(node);
	}

	inline void AddQuad(const Quad& element)
	{
		m_Quads.push_back(element);
	}

	inline void AddLine(const Line& line)
	{
		m_Lines.push_back(line);
	}

private:
	std::vector<Node> m_Nodes;
	std::vector<Quad> m_Quads;
	std::vector<Line> m_Lines;
};

}
