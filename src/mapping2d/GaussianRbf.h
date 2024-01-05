#ifndef MAPPING2D_MAPPING2D_GAUSSIANRBF_H_
#define MAPPING2D_MAPPING2D_GAUSSIANRBF_H_

struct Point;

class MAPPING2D GaussianRbf
{
public:
	GaussianRbf();
	GaussianRbf(double a);
	~GaussianRbf();
	double operator()(const Point& u, const Point& v);

	double a2; //!< a^2, a - parameter
};

#endif // MAPPING2D_MAPPING2D_GAUSSIANRBF_H_