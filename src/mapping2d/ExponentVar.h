#pragma once

struct Point;

struct MAPPING2D ExponentVar
{
	ExponentVar();
	ExponentVar(double c, double c0, double a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	double a; //!< correlation radius
};