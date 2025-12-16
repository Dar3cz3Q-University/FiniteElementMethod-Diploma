#pragma once

#include "math/math.h"

#include <string>

#include <Eigen/Sparse>

namespace fem::cache
{

class MatrixSerializer
{
public:
	static bool SaveSparseMatrix(const std::string& filename, const SpMat& matrix);
	static bool LoadSparseMatrix(const std::string& filename, SpMat& matrix);

	static bool SaveVector(const std::string& filename, const Vec& vector);
	static bool LoadVector(const std::string& filename, Vec& vector);

	static bool SaveSparseMatrices(const std::string& directory, const std::string& prefix, const std::vector<std::pair<std::string, const SpMat*>>& matrices);
	static bool LoadSparseMatrices(const std::string& directory, const std::string& prefix, std::vector<std::pair<std::string, SpMat*>>& matrices);

private:
	MatrixSerializer() = delete;
};

} // namespace fem::cache
