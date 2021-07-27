#pragma once

#include "Structs.h"

class MAPPING2D RegularMesh2d
{
public:
	static RegularMesh2d calcRegularMesh2d(const PointsData& points, size_t nx, size_t ny);
	static RegularMesh2d calcRegularMesh2d(const PointsData& points, double dx, double dy);

public:
	RegularMesh2d();
	RegularMesh2d(const Point& origin, double dx, double dy, size_t nx, size_t ny, double alpha);
	~RegularMesh2d();
	Point getXY(size_t i, size_t j) const;
	Node getIJ(double x, double y) const;
	size_t getNx() const;
	size_t getNy() const;
	double getWidth() const;
	double getHeight() const;
	Point getOrigin() const;
	double getXMin() const;
	double getYMin() const;
	double getXMax() const;
	double getYMax() const;
	double getDx() const;
	double getDy() const;

private:
	Point mOrigin;
	double mDx;
	double mDy;
	size_t mNx;
	size_t mNy;
	double mAlpha;
	double mSinAlpha;
	double mCosAlpha;
};