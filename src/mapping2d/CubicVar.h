#ifndef MAPPING2D_MAPPING2D_CUBICVAR_H_
#define MAPPING2D_MAPPING2D_CUBICVAR_H_

struct Point;

struct MAPPING2D CubicVar
{
	CubicVar();
	CubicVar(double c, double c0, double a);
	double operator()(const Point& u, const Point& v);

	double c; //!< c + c0 = sill
	double c0; //!< nugget
	//! a - correlation radius
	double a2; //!< a^2
	double a3; //!< a^3
	double a5; //!< a^5
	double a7; //!< a^7
};

#endif // MAPPING2D_MAPPING2D_CUBICVAR_H_