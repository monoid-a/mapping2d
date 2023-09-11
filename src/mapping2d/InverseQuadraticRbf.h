#pragma once

struct Point;

class MAPPING2D InverseQuadraticRbf
{
public:
	InverseQuadraticRbf();
	InverseQuadraticRbf(double a);
	~InverseQuadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a2; //!< a^2, a - parameter
};