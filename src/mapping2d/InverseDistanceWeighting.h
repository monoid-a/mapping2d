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
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getVals() const;
	double correctZ(double z) const;

private:
	double m_order;
};
