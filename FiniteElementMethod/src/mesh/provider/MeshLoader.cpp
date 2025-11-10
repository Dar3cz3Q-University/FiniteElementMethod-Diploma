#include "MeshLoader.h"

#include "GmshTypes.h"

#include <iostream>

#include "gmsh.h"

namespace fem::mesh::provider
{

std::expected<model::Mesh, MeshProviderError> MeshLoader::Load(const fs::path& path) const
{
	std::vector<std::size_t> nodeTags;
	std::vector<double> nodeCoords;
	std::vector<double> nodeParams;

	std::vector<int> elemTypes;
	std::vector<std::vector<std::size_t>> elemTags;
	std::vector<std::vector<std::size_t>> elemNodeTags;

	std::vector<std::size_t> boundaryNodes; // TODO: Read boundary nodes

	try
	{
		gmsh::open(path.string());
		gmsh::model::mesh::getNodes(nodeTags, nodeCoords, nodeParams);
		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags);
	}
	catch (...)
	{
		// TODO: Return error
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "" });
	}

	std::size_t quadCount = 0;
	std::size_t lineCount = 0;

	for (std::size_t i = 0; i < elemTypes.size(); i++)
	{
		int type = elemTypes[i];
		if (isQuad(type))
		{
			quadCount += elemTags[i].size();
		}
		else if (isLine(type))
		{
			lineCount += elemTags[i].size();
		}
	}

	model::Mesh mesh(nodeTags.size(), quadCount, lineCount);

	for (std::size_t i = 0; i < nodeTags.size(); i++)
	{
		model::Node n{
			nodeTags[i],
			nodeCoords[3 * i + 0],
			nodeCoords[3 * i + 1],
			nodeCoords[3 * i + 2],
		};

		mesh.AddNode(n);
	}

	for (std::size_t i = 0; i < elemTypes.size(); i++)
	{
		int type = elemTypes[i];
		const auto& tags = elemTags[i];
		const auto& conn = elemNodeTags[i];

		if (isQuad(type))
		{
			for (std::size_t j = 0; j < tags.size(); j++)
			{
				model::Quad q;
				q.id = tags[j];
				q.nodeIDs = {
					conn[4 * j + 0],
					conn[4 * j + 1],
					conn[4 * j + 2],
					conn[4 * j + 3],
				};

				mesh.AddQuad(q);
			}
		}
		else if (isLine(type))
		{
			for (std::size_t j = 0; j < tags.size(); j++)
			{
				model::Line l;
				l.id = tags[j];
				l.nodeIDs = {
					conn[2 * j + 0],
					conn[2 * j + 1],
				};

				mesh.AddLine(l);
			}
		}
	}

	return mesh;
}

inline static bool isLine(int t)
{
	return t == std::to_underlying(GmshElementType::Line2);
}

inline static bool isQuad(int t)
{
	return t == std::to_underlying(GmshElementType::Quad4);
}

}
