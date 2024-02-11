#ifndef MAPPING2D_MAPPING2D_UNIVERSALKRIGING_H_
#define MAPPING2D_MAPPING2D_UNIVERSALKRIGING_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D UniversalKriging : public LinearEstimator<UniversalKriging>, public Variogramer<UniversalKriging>
{
	friend class InterpolAccessor<UniversalKriging>;

public:
	UniversalKriging(const PointsData& data, const TwoPointsFunc& cov);
	UniversalKriging(PointsData&& data, const TwoPointsFunc& cov);
	~UniversalKriging();

protected:
	//interpol accessor
	UblDblMatrix calcMatrix() const;
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	double correctZ(double z) const;

	UblDblVec calcVec(double x, double y, const PointsData& data, const TwoPointsFunc& gamma) const;
};

#endif // MAPPING2D_MAPPING2D_UNIVERSALKRIGING_H_