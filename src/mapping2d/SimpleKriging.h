#ifndef MAPPING2D_MAPPING2D_SIMPLEKRIGING_H_
#define MAPPING2D_MAPPING2D_SIMPLEKRIGING_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D SimpleKriging : public LinearEstimator<SimpleKriging>, public Variogramer<SimpleKriging>
{
	friend class InterpolAccessor<SimpleKriging>;

public:
	SimpleKriging(const PointsData& data, const TwoPointsFunc& cov, double mean);
	SimpleKriging(PointsData&& data, const TwoPointsFunc& cov, double mean);
	~SimpleKriging();

protected:
	// interpol accessor
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	UblDblMatrix calcMatrix() const;
	double correctZ(double z) const;

	UblDblVec calcVec(double x, double y, const PointsData& data, const TwoPointsFunc& gamma) const;

private:
	double mMean;
};

#endif // MAPPING2D_MAPPING2D_SIMPLEKRIGING_H_