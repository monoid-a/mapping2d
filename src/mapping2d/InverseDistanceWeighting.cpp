#include "pch.h"
#include "InverseDistanceWeighting.h"
#include <algorithm>

InverseDistanceWeighting::InverseDistanceWeighting(const PointsData& data, double order) : LinearEstimator(data), mOrder(order)
{

}

InverseDistanceWeighting::InverseDistanceWeighting(PointsData&& data, double order) : LinearEstimator(std::move(data)), mOrder(order)
{

}

InverseDistanceWeighting::~InverseDistanceWeighting()
{

}

std::vector<double> InverseDistanceWeighting::getWeights(double x, double y) const
{
	std::vector<double> inverse_distances;
	int inf_ind = -1;
	const double eps = 1e-5;
	for (size_t i = 0; i < mPointsData.x.size(); ++i)
	{
		double xi = mPointsData.x[i];
		double yi = mPointsData.y[i];

		double d2 = (x - xi) * (x - xi) + (y - yi) * (y - yi);
		double d = std::sqrt(d2);
		if (d < eps)
		{
			d = 1.0;
			inf_ind = (int) i;
		}
		inverse_distances.push_back(1.0 / std::pow(d, mOrder));
	}

	if (inf_ind > -1)
	{
		for (size_t i = 0; i < inverse_distances.size(); ++i)
		{
			if (i != (size_t)inf_ind)
				inverse_distances[i] = 0.0;
		}
	}

	double sum = 0.0;
	std::for_each(inverse_distances.begin(), inverse_distances.end(), [&sum](double val) { sum += val; });

	std::vector<double> vec(mPointsData.x.size(), 0.0);
	for (size_t i = 0, cnt = vec.size(); i < cnt; ++i)
		vec[i] = inverse_distances[i] / sum;

	return vec;
}

std::vector<double> InverseDistanceWeighting::getSampleValues(double x, double y) const
{
	return mPointsData.z;
}

double InverseDistanceWeighting::correctZ(double z) const
{
	return z;
}