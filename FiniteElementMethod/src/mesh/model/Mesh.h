#pragma once

#include "Node.h"
#include "Line.h"
#include "Quad.h"

#include <unordered_map>
#include <vector>

namespace fem::mesh::model
{

class Mesh
{
public:
	Mesh() = default;
	Mesh(size_t numberOfNodes, size_t numberOfCells, size_t numberOfLines);

	inline void AddNode(const Node& node) {
		std::size_t localID = m_Nodes.size();
		m_Nodes.push_back(node);
		m_NodeIndexByGmshId[node.id] = localID;
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

	std::unordered_map<std::size_t, std::size_t> m_NodeIndexByGmshId;
};

}
