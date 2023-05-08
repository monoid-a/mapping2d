#include "pch.h"
#include "RbfInterpolator.h"
#include "MatrixCalculator.h"

RbfInterpolator::RbfInterpolator(const PointsData& data, const two_points_func& cov) : LinearEstimator(data), Variogramer(cov)
{
	calcWeights();
}

RbfInterpolator::RbfInterpolator(PointsData&& data, const two_points_func& cov) : LinearEstimator(std::move(data)), Variogramer(cov)
{

}

RbfInterpolator::~RbfInterpolator()
{

}

UblDblMatrix RbfInterpolator::calcMatrix()
{
	const auto& xs = mPointsData.x;
	const auto& ys = mPointsData.y;
	size_t nx = xs.size();
	size_t ny = ys.size();
	return MatrixCalculator::calcMatrix(xs, ys, nx, ny, mGamma, nullptr);
}

void RbfInterpolator::calcWeights()
{
	if (mWeights.empty())
	{
		UblDblVec f(mPointsData.z.size(), 0.0);
		for (size_t i = 0, p_cnt = mPointsData.z.size(); i < p_cnt; ++i)
			f[i] = mPointsData.z[i];

		auto ws = prod(mInvA, f);

		mWeights.reserve(ws.size());
		for (auto w : ws)
			mWeights.push_back(w);
	}
}

std::vector<double> RbfInterpolator::getWeights(double x, double y) const
{
	return mWeights;
}

std::vector<double> RbfInterpolator::getVals(double x, double y) const
{
	std::vector<double> vec(mPointsData.x.size(), 0.0);

	for (size_t i = 0, p_cnt = mPointsData.x.size(); i < p_cnt; ++i)
		vec[i] = mGamma(Point{ x , y }, Point{ mPointsData.x[i] , mPointsData.y[i] });

	return vec;
}

double RbfInterpolator::correctZ(double z) const
{
	return z;
}