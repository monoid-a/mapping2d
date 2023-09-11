#include "pch.h"
#include "GaussianRbf.h"
#include "Structs.h"

GaussianRbf::GaussianRbf() : a2(1.0)
{
}

GaussianRbf::GaussianRbf(double a) : a2(a * a)
{
}

GaussianRbf::~GaussianRbf()
{

}

double GaussianRbf::operator()(const Point& u, const Point& v)
{
	double h2 = (u.x - v.x) * (u.x - v.x) + (u.y - v.y) * (u.y - v.y);
	double val = exp(-a2 * h2);
	return val;
}