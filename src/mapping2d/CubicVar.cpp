#include "stdafx.h"
#include "CubicVar.h"
#include "Structs.h"

CubicVar::CubicVar()
{
	c = 1.0;
	c0 = 0.0;
	a = 2.0;
	a2 = a * a;
	a3 = a2 * a;
	a5 = a3 * a2;
	a7 = a5 * a2;
}

CubicVar::CubicVar(double _c, double _c0, double _a) : c(_c), c0(_c0), a(_a)
{
	a2 = a * a;
	a3 = a2 * a;
	a5 = a3 * a2;
	a7 = a5 * a2;
}

double CubicVar::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);

	if (h2 < a2)
	{
		double h = sqrt(h2);
		double h3 = h2 * h;
		double h5 = h2 * h3;
		double h7 = h2 * h5;
		double var = c0 + (c - c0) * (
			7.0 * h2 / a2 -
			(35.0 / 4.0) * (h3 / a3) +
			(7.0 / 2.0) * (h5 / a5) -
			(3.0 / 4.0) * (h7 / a7)
			);
		return var;
	}

	return c0 + c;
}