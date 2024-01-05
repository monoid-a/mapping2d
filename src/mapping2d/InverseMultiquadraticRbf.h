#ifndef MAPPING2D_MAPPING2D_INVERSEMULTIQUADRATICRBF_H_
#define MAPPING2D_MAPPING2D_INVERSEMULTIQUADRATICRBF_H_

struct Point;

class MAPPING2D InverseMultiquadraticRbf
{
public:
	InverseMultiquadraticRbf();
	InverseMultiquadraticRbf(double a);
	~InverseMultiquadraticRbf();
	double operator()(const Point& u, const Point& v);

	double a2; //!< a^2, a - parameter
};

#endif // MAPPING2D_MAPPING2D_INVERSEMULTIQUADRATICRBF_H_