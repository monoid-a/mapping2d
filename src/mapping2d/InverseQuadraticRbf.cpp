#include "pch.h"
#include "InverseQuadraticRbf.h"
#include "Structs.h"

InverseQuadraticRbf::InverseQuadraticRbf() : a2(1.0)
{
}

InverseQuadraticRbf::InverseQuadraticRbf(double a) : a2(a* a)
{
}

InverseQuadraticRbf::~InverseQuadraticRbf()
{

}

double InverseQuadraticRbf::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double val = 1.0 / (1.0 + a2 * h2);
	return val;
}