#include "pch.h"
#include "RbfInterpolator.h"
#include "MatrixCalculator.h"

RbfInterpolator::RbfInterpolator(const PointsData& data, const two_points_func& cov) : LinearEstimator(data), Variogramer(cov)
{

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

std::vector<double> RbfInterpolator::getWeights(double x, double y) const
{
	UblDblVec vec(mPointsData.x.size(), 0.0);

	for (size_t i = 0, p_cnt = mPointsData.x.size(); i < p_cnt; ++i)
		vec[i] = mGamma(Point{ x , y }, Point{ mPointsData.x[i] , mPointsData.y[i] });

	return { vec.begin() , vec.end() };
}

std::vector<double> RbfInterpolator::getVals() const
{
	UblDblVec f(mPointsData.z.size(), 0.0);
	for (size_t i = 0, p_cnt = mPointsData.z.size(); i < p_cnt; ++i)
		f[i] = mPointsData.z[i];

	auto w = prod(mInvA, f);

	std::vector<double> vv;
	for (auto v : w)
		vv.push_back(v);

	return vv;
}

double RbfInterpolator::correctZ(double z) const
{
	return z;
}