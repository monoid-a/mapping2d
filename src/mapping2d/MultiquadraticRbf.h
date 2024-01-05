#ifndef MAPPING2D_MAPPING2D_MULTIQUADRATICRBF_H_
#define MAPPING2D_MAPPING2D_MULTIQUADRATICRBF_H_

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

#endif // MAPPING2D_MAPPING2D_MULTIQUADRATICRBF_H_