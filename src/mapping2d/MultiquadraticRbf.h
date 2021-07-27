#pragma once

struct Point;

class MultiquadraticRbf
{
public:
	MultiquadraticRbf();
	MultiquadraticRbf(double _a);
	~MultiquadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a; //!< a - parameter
	double a2; //!< a^2
};