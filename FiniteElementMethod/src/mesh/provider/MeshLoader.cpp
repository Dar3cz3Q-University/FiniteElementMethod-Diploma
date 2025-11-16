#include "MeshLoader.h"

#include "GmshTypes.h"

#include "logger/logger.h"

#include "gmsh.h"

namespace fem::mesh::provider
{

std::expected<model::Mesh, MeshProviderError> MeshLoader::Load(const fs::path& path) const
{
	LOG_INFO("Loading mesh using gmsh from: {}", path.string());

	std::vector<std::size_t> nodeTags;
	std::vector<double> nodeCoords;
	std::vector<double> nodeParams;

	std::vector<int> elemTypes;
	std::vector<std::vector<std::size_t>> elemTags;
	std::vector<std::vector<std::size_t>> elemNodeTags;

	std::vector<std::pair<int, int>> physicalGroups;

	try
	{
		gmsh::open(path.string());

		gmsh::model::mesh::getNodes(nodeTags, nodeCoords, nodeParams);
		gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTags);

		// Get physical groups only for lines (dim=1)
		gmsh::model::getPhysicalGroups(physicalGroups, 1);

		LOG_INFO("Found {} nodes", nodeTags.size());
		LOG_INFO("Found {} element types", elemTypes.size());
		LOG_INFO("Found {} physical groups for lines", physicalGroups.size());
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

	LOG_INFO("Mesh contains: {} quads, {} lines", quadCount, lineCount);

	model::Mesh mesh(nodeTags.size(), quadCount, lineCount);

	for (std::size_t i = 0; i < nodeTags.size(); i++)
	{
		model::Node n{
			nodeTags[i],
			nodeCoords[3 * i + 0],
			nodeCoords[3 * i + 1],
		};

		mesh.AddNode(n);
	}

	LOG_TRACE("Added {} nodes", nodeTags.size());

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

	LOG_TRACE("Added {} quads and {} lines", quadCount, lineCount);

	try
	{
		for (const auto& [dim, tag] : physicalGroups)
		{
			model::PhysicalGroup group;
			group.dimension = dim;
			group.tag = tag;

			gmsh::model::getPhysicalName(dim, tag, group.name);

			LOG_TRACE("Processing physical group: '{}' (tag={}, dim={})", group.name, tag, dim);

			std::vector<int> entities;
			gmsh::model::getEntitiesForPhysicalGroup(dim, tag, entities);

			for (int entity : entities)
			{
				std::vector<int> entElemTypes;
				std::vector<std::vector<std::size_t>> entElemTags;
				std::vector<std::vector<std::size_t>> entElemNodeTags;

				gmsh::model::mesh::getElements(
					entElemTypes,
					entElemTags,
					entElemNodeTags,
					dim,
					entity
				);

				for (const auto& elemTagsVec : entElemTags)
				{
					for (std::size_t lineID : elemTagsVec)
					{
						group.lineIDs.push_back(lineID);
					}
				}
			}

			LOG_INFO("Physical group '{}' contains {} lines", group.name, group.lineIDs.size());

			mesh.AddPhysicalGroup(group);
		}
	}
	catch (...)
	{
		// TODO: Return error
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "" });
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
