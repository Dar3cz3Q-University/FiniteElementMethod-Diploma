#pragma once

#include "Node.h"
#include "Line.h"
#include "Quad.h"
#include "PhysicalGroup.h"

#include <optional>
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
	inline void AddQuad(const Quad& element) { m_Quads.push_back(element); }
	inline void AddLine(const Line& line) {
		std::size_t localID = m_Lines.size();
		m_Lines.push_back(line);
		m_LineIndexByGmshId[line.id] = localID;
	}
	inline void AddPhysicalGroup(const PhysicalGroup& group) { m_PhysicalGroups.push_back(group); }

	inline const std::vector<Node>& GetNodes() const { return m_Nodes; }
	inline const std::vector<Quad>& GetQuads() const { return m_Quads; }
	inline const std::vector<Line>& GetLines() const { return m_Lines; }

	inline const Node& GetNode(const std::size_t id) const
	{
		const auto& index = m_NodeIndexByGmshId.at(id);
		return m_Nodes.at(index);
	}

	inline const Line& GetLine(const std::size_t id) const
	{
		const auto& index = m_LineIndexByGmshId.at(id);
		return m_Lines.at(index);
	}

	inline const std::size_t GetNodesCount() const
	{
		return m_Nodes.size();
	}

	std::optional<PhysicalGroup> GetPhysicalGroupByName(const std::string_view& name) const;

private:
	std::vector<Node> m_Nodes;
	std::vector<Quad> m_Quads;
	std::vector<Line> m_Lines;
	std::vector<PhysicalGroup> m_PhysicalGroups;

	std::unordered_map<std::size_t, std::size_t> m_NodeIndexByGmshId;
	std::unordered_map<std::size_t, std::size_t> m_LineIndexByGmshId;
};

}
