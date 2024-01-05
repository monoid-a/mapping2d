#ifndef MAPPING2D_MAPPING2D_INVERSEDISTANCEWEIGHTING_H_
#define MAPPING2D_MAPPING2D_INVERSEDISTANCEWEIGHTING_H_

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D InverseDistanceWeighting : public LinearEstimator<InverseDistanceWeighting>
{
	friend class InterpolAccessor<InverseDistanceWeighting>;

public:
	InverseDistanceWeighting(const PointsData& data, double order);
	InverseDistanceWeighting(PointsData&& data, double order);
	~InverseDistanceWeighting();

protected:
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getSampleValues(double x, double y) const;
	double correctZ(double z) const;

private:
	double m_order;
};

#endif // MAPPING2D_MAPPING2DINVERSEDISTANCEWEIGHTING_H_