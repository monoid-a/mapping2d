#pragma once

#include <functional>
#include <vector>
#include <deque>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

struct Point;
using two_points_func = std::function<double(const Point& u, const Point& v)>;

namespace ubl = boost::numeric::ublas;
using UblDblMatrix = ubl::matrix<double, ubl::row_major, std::vector<double>>;
using UblDblVec = ubl::vector<double, std::vector<double>>;

using modif_matrix = std::function<void(UblDblMatrix& matr)>;
using modif_vector = std::function<void(UblDblVec& matr)>;

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
	friend bool operator==(const Point& l, const Point& r)
	{
		return l.x == r.x && l.y == r.y;
	}

	friend bool operator!=(const Point& l, const Point& r)
	{
		return !(l == r);
	}

	friend Point operator-(const Point& l, const Point& r)
	{
		return Point{ l.x - r.x, l.y - r.y };
	}

	friend bool isEq(const Point& v, const Point& p, double eps)
	{
		return std::abs(v.x - p.x) < eps && std::abs(v.y - p.y) < eps;
	}
};

struct Cell
{
	Point bottom_left;
	Point top_left;
	Point bottom_right;
	Point top_right;
};

struct Node
{
	size_t i;
	size_t j;

	bool operator==(const Node& other) const
	{
		return std::tie(i, j) == std::tie(other.i, other.j);
	}

	bool operator!=(const Node& other) const
	{
		return std::tie(i, j) != std::tie(other.i, other.j);
	}

	bool operator<(const Node& other) const
	{
		return std::tie(i, j) < std::tie(other.i, other.j);
	}
};

struct Segment
{
	Segment()
	{
	}

	Segment(const Point& bgn, const Point& end) : bgn(bgn), end(end)
	{
	}

	bool isBound(const Point& p)
	{
		return isEq(bgn, p, 1e-6) || isEq(end, p, 1e-6);
	}

	Point bgn;
	Point end;
};