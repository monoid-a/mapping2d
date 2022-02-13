#pragma once

struct Point;

class MAPPING2D GaussianRbf
{
public:
	GaussianRbf();
	GaussianRbf(double _a);
	~GaussianRbf();
	double operator()(const Point& u, const Point& v);

	double a; //!< a - parameter
	double a2; //!< a^2
};