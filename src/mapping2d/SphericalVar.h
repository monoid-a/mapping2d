#pragma once

struct Point;

struct MAPPING2D SphericalVar
{
	SphericalVar();
	SphericalVar(double c, double c0, double a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	double a; //!< correlation radius
	double a2; //!< a^2
	double a3; //!< a^3
};