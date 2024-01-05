#ifndef MAPPING2D_MAPPING2D_GAUSSVAR_H_
#define MAPPING2D_MAPPING2D_GAUSSVAR_H_

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

#endif // MAPPING2D_MAPPING2D_GAUSSVAR_H_