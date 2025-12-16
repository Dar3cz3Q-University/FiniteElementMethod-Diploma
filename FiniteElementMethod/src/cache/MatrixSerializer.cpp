#include "MatrixSerializer.h"

#include "logger/logger.h"

#include <unsupported/Eigen/SparseExtra>
#include <filesystem>

// TODO: Use std::expected and fileio module
namespace fem::cache
{

namespace fs = std::filesystem;

bool MatrixSerializer::SaveSparseMatrix(const std::string& filename, const SpMat& matrix)
{
	fs::create_directories(fs::path(filename).parent_path());

	if (!Eigen::saveMarket(matrix, filename))
	{
		LOG_ERROR("Failed to save matrix: {}", filename);
		return false;
	}

	LOG_TRACE("Matrix saved: {} ({}x{}, {} nonzeros, {:.2f} MB)",
		filename, matrix.rows(), matrix.cols(), matrix.nonZeros(),
		fs::file_size(filename) / (1024.0 * 1024.0));

	return true;
}

bool MatrixSerializer::LoadSparseMatrix(const std::string& filename, SpMat& matrix)
{
	if (!fs::exists(filename))
	{
		LOG_TRACE("Matrix file not found: {}", filename);
		return false;
	}

	if (!Eigen::loadMarket(matrix, filename))
	{
		LOG_ERROR("Failed to load matrix: {}", filename);
		return false;
	}

	LOG_TRACE("Matrix loaded: {} ({}x{}, {} nonzeros)",
		filename, matrix.rows(), matrix.cols(), matrix.nonZeros());

	return true;
}

bool MatrixSerializer::SaveVector(const std::string& filename, const Vec& vector)
{
	fs::create_directories(fs::path(filename).parent_path());

	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open file for writing: {}", filename);
		return false;
	}

	size_t size = vector.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	file.write(reinterpret_cast<const char*>(vector.data()), size * sizeof(double));

	file.close();
	LOG_TRACE("Vector saved: {} ({} elements, {:.2f} KB)",
		filename, size, fs::file_size(filename) / 1024.0);

	return true;
}

bool MatrixSerializer::LoadVector(const std::string& filename, Vec& vector)
{
	if (!fs::exists(filename))
	{
		LOG_TRACE("Vector file not found: {}", filename);
		return false;
	}

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open file for reading: {}", filename);
		return false;
	}

	size_t size;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));

	vector.resize(size);
	file.read(reinterpret_cast<char*>(vector.data()), size * sizeof(double));

	file.close();
	LOG_TRACE("Vector loaded: {} ({} elements)", filename, size);

	return true;
}

bool MatrixSerializer::SaveSparseMatrices(
	const std::string& directory,
	const std::string& prefix,
	const std::vector<std::pair<std::string, const SpMat*>>& matrices)
{
	for (const auto& [name, matrix] : matrices)
	{
		std::string filename = std::format("{}/{}_{}.mtx", directory, prefix, name);
		if (!SaveSparseMatrix(filename, *matrix))
		{
			return false;
		}
	}

	return true;
}

bool MatrixSerializer::LoadSparseMatrices(
	const std::string& directory,
	const std::string& prefix,
	std::vector<std::pair<std::string, SpMat*>>& matrices)
{
	for (auto& [name, matrix] : matrices)
	{
		std::string filename = std::format("{}/{}_{}.mtx", directory, prefix, name);
		if (!LoadSparseMatrix(filename, *matrix))
		{
			return false;
		}
	}

	return true;
}

} // namespace fem::cache
