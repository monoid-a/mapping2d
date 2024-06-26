#ifndef MAPPING2D_MAPPING2D_REGULARMESH2D_H_
#define MAPPING2D_MAPPING2D_REGULARMESH2D_H_

#include "Structs.h"

class MAPPING2D RegularMesh2d
{
public:
	static RegularMesh2d calculate(const PointsData& points, size_t nx, size_t ny);
	static RegularMesh2d calculate(const PointsData& points, double dx, double dy);
	static RegularMesh2d calculate(size_t nx, size_t ny, double stepx, double stepy, double origx, double origy, double angle);

public:
	RegularMesh2d();
	RegularMesh2d(const Point& origin, double dx, double dy, size_t nx, size_t ny, double alpha);
	RegularMesh2d(const RegularMesh2d& mesh);
	~RegularMesh2d();
	Point getXY(size_t i, size_t j) const;
	Node getIJ(double x, double y) const;
	Cell getCellPoints(size_t i, size_t j) const;
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
	double getAngle() const;
	void rotate(double x, double y, double& tx, double& ty);
	void rotateInv(double x, double y, double& tx, double& ty);

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

#endif // MAPPING2D_MAPPING2D_REGULARMESH2D_H_