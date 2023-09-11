#pragma once

struct Point;

struct MAPPING2D GaussVar
{
	GaussVar();
	GaussVar(double c, double c0, double a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	//! a - correlation radius
	double a2; //!< a^2
};