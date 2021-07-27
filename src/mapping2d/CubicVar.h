#pragma once

struct Point;

struct CubicVar
{
	CubicVar();
	CubicVar(double _c, double _c0, double _a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	double a; //!< correlation radius
	double a2; //!< a^2
	double a3; //!< a^3
	double a5; //!< a^5
	double a7; //!< a^7
};