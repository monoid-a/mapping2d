#pragma once

struct Point;

class MAPPING2D ThinPlateSplineRbf
{
public:
	ThinPlateSplineRbf(double smoothParam);
	~ThinPlateSplineRbf();
	double operator()(const Point& u, const Point& v);

protected:
	double mSmoothParam;
};