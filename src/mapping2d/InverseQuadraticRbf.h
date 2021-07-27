#pragma once

struct Point;

class InverseQuadraticRbf
{
public:
	InverseQuadraticRbf();
	InverseQuadraticRbf(double _a);
	~InverseQuadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a; //!< a - parameter
	double a2; //!< a^2
};