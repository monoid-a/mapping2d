#include "stdafx.h"
#include "MultiquadraticRbf.h"
#include "Structs.h"

MultiquadraticRbf::MultiquadraticRbf()
{
	a = 1.0;
	a2 = 1.0;
}

MultiquadraticRbf::MultiquadraticRbf(double _a) : a(_a)
{
	a2 = a * a;
}

MultiquadraticRbf::~MultiquadraticRbf()
{

}

double MultiquadraticRbf::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double val = sqrt(1.0 + a2 * h2);
	return val;
}