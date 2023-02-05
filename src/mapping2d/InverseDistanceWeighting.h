#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D InverseDistanceWeighting : public LinearEstimator<InverseDistanceWeighting>
{
	friend class InterpolAccessor;

public:
	InverseDistanceWeighting(const PointsData& data, double order);
	InverseDistanceWeighting(PointsData&& data, double order);
	~InverseDistanceWeighting();

protected:
	std::vector<double> getWeights(double x, double y);
	std::vector<double> getVals();
	double correctZ(double z);

private:
	double m_order;
};
