#include "MeshGenerator.h"

#include "gmsh.h"

namespace fem::mesh::provider
{

std::expected<model::Mesh, MeshProviderError> MeshGenerator::GenerateFromGeo(const fs::path& path) const
{
	auto mshPath = GenerateMshWithGmsh(path);

	if (!mshPath)
		return std::unexpected(mshPath.error());

	return m_Loader.Load(*mshPath);
}

std::expected<fs::path, MeshProviderError> MeshGenerator::GenerateMshWithGmsh(const fs::path& path) const
{
	std::error_code ec;

	if (!fs::exists(path, ec))
	{
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::InputPathNotFound, ec.message() });
	}

	fs::path resultPath = path;
	resultPath.replace_extension(".msh");

	Stage stage = Stage::Open;

	try
	{
		gmsh::open(path.string());

		stage = Stage::Generate;
		gmsh::model::mesh::generate(2);

		stage = Stage::Write;
		gmsh::write(resultPath.string());
	}
	catch (const std::bad_alloc& ex)
	{
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::OutOfMemory, ex.what() });
	}
	catch (const std::filesystem::filesystem_error& ex)
	{
		MeshProviderErrorCode code = MeshProviderErrorCode::IoError;
		if (stage == Stage::Write)
			code = MeshProviderErrorCode::OutputWriteFailed;

		return std::unexpected(MeshProviderError{ code, ex.what() });
	}
	catch (const std::exception& ex)
	{
		MeshProviderErrorCode code = MeshProviderErrorCode::GmshOpenFailed;
		if (stage == Stage::Generate)
			code = MeshProviderErrorCode::GmshGenerateFailed;
		else if (stage == Stage::Write)
			code = MeshProviderErrorCode::GmshWriteFailed;

		return std::unexpected(MeshProviderError{ code, ex.what() });
	}
	catch (...)
	{
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::Unknown, "Unknwon exception" });
	}

	if (!fs::exists(resultPath, ec))
	{
		return std::unexpected(MeshProviderError{ MeshProviderErrorCode::ResultMissing, ec.message() });
	}

	return resultPath;
}

}
