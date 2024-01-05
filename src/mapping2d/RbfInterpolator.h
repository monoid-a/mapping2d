#ifndef MAPPING2D_MAPPING2D_RBFINTERPOLATOR_H_
#define MAPPING2D_MAPPING2D_RBFINTERPOLATOR_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D RbfInterpolator : public LinearEstimator<RbfInterpolator>, public Variogramer<RbfInterpolator>
{
	friend class InterpolAccessor<RbfInterpolator>;

public:
	RbfInterpolator(const PointsData& data, const TwoPointsFunc& cov);
	RbfInterpolator(PointsData&& data, const TwoPointsFunc& cov);
	~RbfInterpolator();

protected:
	void calcWeights();
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	double correctZ(double z) const;

	UblDblMatrix calcMatrix() const;

protected:
	std::vector<double> mWeights;
};

#endif // MAPPING2D_MAPPING2D_RBFINTERPOLATOR_H_