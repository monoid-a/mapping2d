#ifndef MAPPING2D_MAPPING2D_THINPLATESPLINE_H_
#define MAPPING2D_MAPPING2D_THINPLATESPLINE_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D ThinPlateSpline : public LinearEstimator<ThinPlateSpline>, public Variogramer<ThinPlateSpline>
{
	friend class InterpolAccessor<ThinPlateSpline>;

public:
	ThinPlateSpline(const PointsData& data, double smoothParam);
	ThinPlateSpline(PointsData&& data, double smoothParam);
	~ThinPlateSpline();

protected:
	//interpol accessor
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	
	UblDblMatrix calcMatrix() const;
	double correctZ(double z) const;
	void calcWeights();

protected:
	std::vector<double> mWeights;
};

#endif // MAPPING2D_MAPPING2D_THINPLATESPLINE_H_