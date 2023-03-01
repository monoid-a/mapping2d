#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D RbfInterpolator : public LinearEstimator<RbfInterpolator>, public Variogramer<RbfInterpolator>
{
	friend class InterpolAccessor;

public:
	RbfInterpolator(const PointsData& data, const two_points_func& cov);
	RbfInterpolator(PointsData&& data, const two_points_func& cov);
	~RbfInterpolator();

protected:
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getVals() const;
	double correctZ(double z) const;

	UblDblMatrix calcMatrix();
};