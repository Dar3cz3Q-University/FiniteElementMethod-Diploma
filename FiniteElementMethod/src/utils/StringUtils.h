#include <sstream>

#include "Eigen/Dense"

namespace fem
{

template<typename Derived>
std::string EigenToString(const Eigen::MatrixBase<Derived>& mat)
{
    std::ostringstream oss;
    oss << mat;
    return oss.str();
}

}
