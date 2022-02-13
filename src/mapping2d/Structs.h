#pragma once

#include <functional>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

struct Point;
using two_points_func = std::function<double(const Point& u, const Point& v)>;

namespace ubl = boost::numeric::ublas;
using UblDblMatrix = ubl::matrix<double, ubl::row_major, std::vector<double>>;
using UblDblVec = ubl::vector<double, std::vector<double>>;

using matrix_modif = std::function<void(UblDblMatrix& matr)>;
using vector_modif = std::function<void(UblDblVec& matr)>;

struct PointsData
{
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> z;
};

struct Point
{
	double x;
	double y;
};

struct Node
{
	size_t i;
	size_t j;
};