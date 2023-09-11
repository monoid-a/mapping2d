#include "pch.h"
#include "SphericalVar.h"
#include "Structs.h"

SphericalVar::SphericalVar()
{
	c = 1.0;
	c0 = 0.0;
	a = 2.0;
	a2 = a * a;
	a3 = a2 * a;
}

SphericalVar::SphericalVar(double c, double c0, double a) : c(c), c0(c0), a(a), a2(a* a), a3(a2* a)
{
}

double SphericalVar::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	if (h2 < a2)
	{
		double h = sqrt(h2);
		double var = c0 + c * (1.5 * h / a - 0.5 * (h2 * h / a3));
		return var;
	}

	return c0 + c;
}