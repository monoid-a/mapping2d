#pragma once

struct Point;

class MAPPING2D MultiquadraticRbf
{
public:
	MultiquadraticRbf();
	MultiquadraticRbf(double a);
	~MultiquadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a2; //!< a^2, a - parameter
};