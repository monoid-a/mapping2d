#include "pch.h"
#include "ExponentVar.h"
#include "Structs.h"

ExponentVar::ExponentVar()
{
	c = 1.0;
	c0 = 0.0;
	a = 2.0;
}

ExponentVar::ExponentVar(double _c, double _c0, double _a) : c(_c), c0(_c0), a(_a)
{
}

double ExponentVar::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double h = sqrt(h2);
	if (h < 1.0e-9)
		return 0.0;
	double var = c0 + (c - c0) * (1.0 - exp(-3.0 * h / a));
	return var;
}