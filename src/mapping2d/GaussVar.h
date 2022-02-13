#pragma once

struct Point;

struct MAPPING2D GaussVar
{
	GaussVar();
	GaussVar(double _c, double _c0, double _a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	double a; //!< correlation radius
	double a2; //!< a^2
};