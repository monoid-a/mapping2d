#ifndef MAPPING2D_MAPPING2D_METHODSETTINGS_H_
#define MAPPING2D_MAPPING2D_METHODSETTINGS_H_

enum class Method
{
	SimpleKriging,
	OrdinaryKriging,
	UniversalKriging,
	RBF,
	InverseDistanceWeighting,
	ThinPlateSpline,
};

enum class Function
{
	Spherical,
	Cubic,
	Exponent,
	Gauss,
	RbfGauss,
	RbfMultiquadratic,
	RbfInvQuadratic,
	RbfInvMultiquadratic,
};

struct MethodSettings
{
	Method methodType;
	Function funcType;
	double c0; // c0 - nugget
	double c; // c + c0 - sill
	double a; // correlation radius
	double mean; // mean value
	double exponent; // IDW exponent
	double gamma; // RBF parameter
	double smoothParam; // spline smoothing parameter
};

#endif // MAPPING2D_MAPPING2D_METHODSETTINGS_H_