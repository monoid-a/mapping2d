#include "pch.h"
#include "ExponentVar.h"
#include "Structs.h"

ExponentVar::ExponentVar()
{
	c = 1.0;
	c0 = 0.0;
	a = 2.0;
}

ExponentVar::ExponentVar(double c, double c0, double a) : c(c), c0(c0), a(a)
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