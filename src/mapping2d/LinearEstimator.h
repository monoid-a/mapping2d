#ifndef MAPPING2D_MAPPING2D_LINEARESTIMATOR_H_
#define MAPPING2D_MAPPING2D_LINEARESTIMATOR_H_

#include "Structs.h"

template<typename Impl>
class LinearEstimator
{
public:
	LinearEstimator() = delete;

	explicit LinearEstimator(const PointsData& data) : mPointsData(data)
	{
	}

	explicit LinearEstimator(PointsData&& data) : mPointsData(std::move(data))
	{
	}

	~LinearEstimator()
	{
	}

	double getZ(double x, double y) const
	{
		const Impl* impl = static_cast<const Impl*>(this);

		const std::vector<double>& w = InterpolAccessor<Impl>::getWeights(impl, x, y);
		const std::vector<double>& v = InterpolAccessor<Impl>::getSampleValues(impl, x, y);

		// z += w[i] * v[i];
		double z = std::inner_product(v.begin(), v.end(), w.begin(), 0.0);
		z = InterpolAccessor<Impl>::correctZ(impl, z);

		return z;
	}

protected:
	PointsData mPointsData;
};

#endif // MAPPING2D_MAPPING2D_LINEARESTIMATOR_H_