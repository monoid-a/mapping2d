#include "pch.h"
#include "InverseMultiquadraticRbf.h"
#include "Structs.h"

InverseMultiquadraticRbf::InverseMultiquadraticRbf()
{
	a = 1.0;
	a2 = 1.0;
}

InverseMultiquadraticRbf::InverseMultiquadraticRbf(double _a) : a(_a)
{
	a2 = a * a;
}

InverseMultiquadraticRbf::~InverseMultiquadraticRbf()
{

}

double InverseMultiquadraticRbf::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double val = 1.0 / sqrt(1.0 + a2 * h2);
	return val;
}