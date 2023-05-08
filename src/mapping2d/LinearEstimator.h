#pragma once

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

		double z{ 0.0 };

		const std::vector<double>& w = InterpolAccessor::getWeights(impl, x, y);
		const std::vector<double>& v = InterpolAccessor::getVals(impl, x, y);

		for (size_t i = 0; i < v.size(); ++i)
			z += w[i] * v[i];

		z = InterpolAccessor::correctZ(impl, z);

		return z;
	}

protected:
	PointsData mPointsData;
};