#ifndef MAPPING2D_MAPPING2D_THINPLATESPLINERBF_H_
#define MAPPING2D_MAPPING2D_THINPLATESPLINERBF_H_

struct Point;

class MAPPING2D ThinPlateSplineRbf
{
public:
	ThinPlateSplineRbf(double smoothParam);
	~ThinPlateSplineRbf();
	double operator()(const Point& u, const Point& v);

protected:
	double mSmoothParam;
};

#endif // MAPPING2D_MAPPING2D_THINPLATESPLINERBF_H_