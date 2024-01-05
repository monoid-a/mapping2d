#include "pch.h"
#include "SimpleKriging.h"
#include "Structs.h"
#include "MatrixCalculator.h"
#include "VectorCalculator.h"

SimpleKriging::SimpleKriging(const PointsData& data, const TwoPointsFunc& cov, double mean) : LinearEstimator(data), Variogramer(cov), mMean(mean)
{
}

SimpleKriging::SimpleKriging(PointsData&& data, const TwoPointsFunc& cov, double mean) : LinearEstimator(std::move(data)), Variogramer(cov), mMean(mean)
{
}

SimpleKriging::~SimpleKriging()
{
}

UblDblVec SimpleKriging::calcVec(double x, double y, const PointsData& data, const TwoPointsFunc& gamma) const
{
	return VectorCalculator::calculate(x, y, data, data.x.size(), gamma, nullptr);
}

std::vector<double> SimpleKriging::getWeights(double x, double y) const
{
	UblDblVec b = calcVec(x, y, mPointsData, mGamma);
	UblDblVec w = prod(mInvA, b);
	return { w.begin() , w.end() };
}

std::vector<double> SimpleKriging::getSampleValues(double x, double y) const
{
	std::vector<double> res;
	res.reserve(mPointsData.z.size());
	for (auto z : mPointsData.z)
		res.push_back(z - mMean);
	return res;
}

UblDblMatrix SimpleKriging::calcMatrix() const
{
	const auto& xs = mPointsData.x;
	const auto& ys = mPointsData.y;
	size_t nx = xs.size();
	size_t ny = ys.size();
	return MatrixCalculator::calcMatrix(xs, ys, nx, ny, mGamma, nullptr);
}

double SimpleKriging::correctZ(double z) const
{
	return z + mMean;
}