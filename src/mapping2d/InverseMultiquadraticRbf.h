#pragma once

struct Point;

class InverseMultiquadraticRbf
{
public:
	InverseMultiquadraticRbf();
	InverseMultiquadraticRbf(double _a);
	~InverseMultiquadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a; //!< a - parameter
	double a2; //!< a^2
};