#include "stdafx.h"
#include "GaussVar.h"
#include "Structs.h"

GaussVar::GaussVar()
{
	c = 1.0;
	c0 = 0.0;
	a = 2.0;
	a2 = a * a;
}

GaussVar::GaussVar(double _c, double _c0, double _a) : c(_c), c0(_c0), a(_a)
{
	a2 = a * a;
}

double GaussVar::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double var = c0 + c * (1.0 - exp(-3.0 * h2 / a2));
	return var;
}