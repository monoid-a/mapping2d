#include "pch.h"
#include "RegularMesh2d.h"
#include "Structs.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Isoliner.h"

RegularMesh2d RegularMesh2d::calculate(const PointsData& points, size_t nx, size_t ny)
{
	auto xminmax = std::minmax_element(points.x.begin(), points.x.end());
	double offx = 1;
	if (points.x.size() > 1)
		offx = 0.5 * (*xminmax.second - *xminmax.first) / nx;
	double xMin = (*xminmax.first) - offx;
	double xMax = (*xminmax.second) + offx;

	auto yminmax = std::minmax_element(points.y.begin(), points.y.end());
	double offy = 1;
	if (points.y.size() > 1)
		offy = 0.5 * (*yminmax.second - *yminmax.first) / ny;
	double yMin = (*yminmax.first) - offy;
	double yMax = (*yminmax.second) + offy;

	Point origin;
	origin.x = xMin;
	origin.y = yMin;

	double dx = (xMax - xMin) / (nx - 1);
	double dy = (yMax - yMin) / (ny - 1);

	RegularMesh2d mesh(origin, dx, dy, nx, ny, 0.0);

	return mesh;
}

RegularMesh2d RegularMesh2d::calculate(const PointsData& points, double dx, double dy)
{
	auto xminmax = std::minmax_element(points.x.begin(), points.x.end());
	double xMin = (*xminmax.first) - dx;
	double xMax = (*xminmax.second) + dx;

	auto yminmax = std::minmax_element(points.y.begin(), points.y.end());
	double yMin = (*yminmax.first) - dy;
	double yMax = (*yminmax.second) + dy;

	Point origin;
	origin.x = xMin;
	origin.y = yMin;

	size_t nx = (size_t)((xMax - xMin) / dx) + 1;
	size_t ny = (size_t)((yMax - yMin) / dy) + 1;

	RegularMesh2d mesh(origin, dx, dy, nx, ny, 0.0);

	return mesh;
}

RegularMesh2d::RegularMesh2d() : 
	mOrigin{}, mDx{ 0.0 }, mDy{ 0.0 }, mNx{ 0 }, mNy{ 0 }, mAlpha{ 0.0 }, mSinAlpha{ 0.0 }, mCosAlpha{ 0.0 }
{

}

RegularMesh2d::RegularMesh2d(const Point& origin, double dx, double dy, size_t nx, size_t ny, double alpha) :
	mOrigin(origin), mDx(dx), mDy(dy),
	mNx(nx), mNy(ny), mAlpha(alpha),
	mSinAlpha(std::sin(mAlpha* M_PI / 180.0)),
	mCosAlpha(std::cos(mAlpha* M_PI / 180.0))
{
}

RegularMesh2d::RegularMesh2d(const RegularMesh2d& mesh)	: 
	mOrigin{ mesh.mOrigin }, 
	mDx{ mesh.mDx }, mDy{ mesh.mDy },
	mNx{ mesh.mNx }, mNy{ mesh.mNy },
	mAlpha{ mesh.mAlpha }, 
	mSinAlpha{ mesh.mSinAlpha }, mCosAlpha{ mesh.mCosAlpha }
{

}

RegularMesh2d::~RegularMesh2d()
{

}

Point RegularMesh2d::getXY(size_t i, size_t j) const
{
	double x = i * mDx;
	double y = j * mDy;

	Point res;
	res.x = mOrigin.x + x * mCosAlpha - y * mSinAlpha;
	res.y = mOrigin.y + x * mSinAlpha + y * mCosAlpha;

	return res;
}

Node RegularMesh2d::getIJ(double x, double y) const
{
	double x_prime = x - mOrigin.x;
	double y_prime = y - mOrigin.y;

	x = x_prime * mCosAlpha + y_prime * mSinAlpha;
	y = -x_prime * mSinAlpha + y_prime * mCosAlpha;

	Node res;
	res.i = size_t((x + 0.5 * mDx) / mDx);
	res.j = size_t((y + 0.5 * mDy) / mDy);

	if (res.i >= mNx)
		res.i = mNx - 1;

	if (res.j >= mNy)
		res.j = mNy - 1;

	return res;
}

Cell RegularMesh2d::getCellPoints(size_t i, size_t j) const
{
	Cell res;
	res.bottom_left = getXY(i, j);
	res.top_left = getXY(i, j + 1);
	res.bottom_right = getXY(i + 1, j);
	res.top_right = getXY(i + 1, j + 1);
	return res;
}

size_t RegularMesh2d::getNx() const
{
	return mNx;
}

size_t RegularMesh2d::getNy() const
{
	return mNy;
}

double RegularMesh2d::getWidth() const
{
	return (mNx - 1) * mDx;
}

double RegularMesh2d::getHeight() const
{
	return (mNy - 1) * mDy;
}

Point RegularMesh2d::getOrigin() const
{
	return mOrigin;
}

double RegularMesh2d::getXMin() const
{
	return getXY(0, 0).x;
}

double RegularMesh2d::getYMin() const
{
	return getXY(0, 0).y;
}

double RegularMesh2d::getXMax() const
{
	return getXY(getNx() - 1, getNy() - 1).x;
}

double RegularMesh2d::getYMax() const
{
	return getXY(getNx() - 1, getNy() - 1).y;
}

double RegularMesh2d::getDx() const
{
	return mDx;
}

double RegularMesh2d::getDy() const
{
	return mDy;
}

std::vector<Point> RegularMesh2d::getCorners() const
{
	std::vector<Point> corners;
	double xmax = getXMax();
	double xmin = getXMin();
	double ymax = getYMax();
	double ymin = getYMin();
	corners.push_back({ xmin , ymin });
	corners.push_back({ xmax , ymin });
	corners.push_back({ xmax , ymax });
	corners.push_back({ xmin , ymax });
	return corners;
}

bool RegularMesh2d::pointOnBorder(Point point, double eps) const
{
	double xmax = getXMax();
	double xmin = getXMin();
	double ymax = getYMax();
	double ymin = getYMin();

	return std::abs(point.x - xmin) < 1e-5 || std::abs(point.x - xmax) < 1e-5 || 
		std::abs(point.y - ymin) < 1e-5 || std::abs(point.y - ymax) < 1e-5;
}