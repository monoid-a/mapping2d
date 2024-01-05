#ifndef MAPPING2D_MAPPING2D_ORDINARYKRIGING_H_
#define MAPPING2D_MAPPING2D_ORDINARYKRIGING_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D OrdinaryKriging : public LinearEstimator<OrdinaryKriging>, public Variogramer<OrdinaryKriging>
{
	friend class InterpolAccessor<OrdinaryKriging>;

public:
	OrdinaryKriging(const PointsData& data, const TwoPointsFunc& cov);
	OrdinaryKriging(PointsData&& data, const TwoPointsFunc& cov);
	~OrdinaryKriging();

protected:
	// interpol accessor
	UblDblMatrix calcMatrix() const;
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	double correctZ(double z) const;

	UblDblVec calcVec(double x, double y, const PointsData& data, const TwoPointsFunc& gamma) const;
};

#endif // MAPPING2D_MAPPING2D_ORDINARYKRIGING_H_