#include "matrix_io.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <unsupported/Eigen/SparseExtra>

namespace fem::solver::standalone
{

	bool LoadMatrixMarket(const std::string& filename, SpMat& matrix)
	{
		if (!Eigen::loadMarket(matrix, filename))
		{
			std::cerr << "Cannot load matrix: " << filename << std::endl;
			return false;
		}

		std::cout << "Matrix loaded: " << matrix.rows() << "x" << matrix.cols()
			<< ", " << matrix.nonZeros() << " nnz" << std::endl;
		return true;
	}

	bool LoadVector(const std::string& filename, Vec& vec)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Cannot open: " << filename << std::endl;
			return false;
		}

		std::vector<double> values;
		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '%' || line[0] == '#')
				continue;
			std::istringstream iss(line);
			double val;
			while (iss >> val)
				values.push_back(val);
		}

		vec = Eigen::Map<Vec>(values.data(), values.size());
		std::cout << "Vector loaded: " << vec.size() << " elements" << std::endl;
		return true;
	}

	bool SaveVector(const std::string& filename, const Vec& vec)
	{
		std::ofstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Cannot open: " << filename << std::endl;
			return false;
		}

		file << std::scientific << std::setprecision(15);
		for (Eigen::Index i = 0; i < vec.size(); ++i)
			file << vec[i] << "\n";

		std::cout << "Saved: " << vec.size() << " elements to " << filename << std::endl;
		return true;
	}

} // namespace fem::solver::standalone
