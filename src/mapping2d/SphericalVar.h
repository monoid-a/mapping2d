#ifndef MAPPING2D_MAPPING2D_SPHERICALVAR_H_
#define MAPPING2D_MAPPING2D_SPHERICALVAR_H_

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

#endif // MAPPING2D_MAPPING2D_SPHERICALVAR_H_