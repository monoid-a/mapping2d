#include "pch.h"
#include "ThinPlateSplineRbf.h"
#include "Structs.h"

ThinPlateSplineRbf::ThinPlateSplineRbf()
{
}


ThinPlateSplineRbf::~ThinPlateSplineRbf()
{
}

double ThinPlateSplineRbf::operator()(const Point& u, const Point& v)
{
	double r2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	if (r2 > 1e-10)
		return 0.5 * r2 * log(r2);
	return 0.0;
}