#ifndef MAPPING2D_MAPPING2D_INVERSEQUADRATICRBF_H_
#define MAPPING2D_MAPPING2D_INVERSEQUADRATICRBF_H_

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

#endif // MAPPING2D_MAPPING2D_INVERSEQUADRATICRBF_H_