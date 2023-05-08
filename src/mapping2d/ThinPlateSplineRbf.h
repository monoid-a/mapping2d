#pragma once

struct Point;

class MAPPING2D ThinPlateSplineRbf
{
public:
	ThinPlateSplineRbf();
	~ThinPlateSplineRbf();
	double operator()(const Point& u, const Point& v);
};