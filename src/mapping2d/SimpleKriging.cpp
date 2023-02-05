#include "pch.h"
#include "SimpleKriging.h"
#include "Structs.h"
#include "MatrixCalculator.h"
#include "VectorCalculator.h"

SimpleKriging::SimpleKriging(const PointsData& data, const two_points_func& cov, double mean) : LinearEstimator(data), Variogramer(cov), m_mean(mean)
{
}

SimpleKriging::SimpleKriging(PointsData&& data, const two_points_func& cov, double mean) : LinearEstimator(std::move(data)), Variogramer(cov), m_mean(mean)
{
}

SimpleKriging::~SimpleKriging()
{
}

UblDblVec SimpleKriging::calcVec(double x, double y, const PointsData& data, const two_points_func& gamma)
{
	return VectorCalculator::calculate(x, y, data, data.x.size(), gamma, nullptr);
}

UblDblVec SimpleKriging::calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b)
{
	auto w = prod(A_inv, b);
	return w;
}

std::vector<double> SimpleKriging::getWeights(double x, double y)
{
	UblDblVec b = calcVec(x, y, mPointsData, mGamma);
	UblDblVec w = calcWeights(mInvA, b);
	return { w.begin() , w.end() };
}

std::vector<double> SimpleKriging::getVals()
{
	std::vector<double> res;
	for (auto z : mPointsData.z)
		res.push_back(z - m_mean);
	return res;
}

UblDblMatrix SimpleKriging::calcMatrix()
{
	const auto& xs = mPointsData.x;
	const auto& ys = mPointsData.y;
	size_t nx = xs.size();
	size_t ny = ys.size();
	return MatrixCalculator::calcMatrix(xs, ys, nx, ny, mGamma, nullptr);
}

double SimpleKriging::correctZ(double z)
{
	return z + m_mean;
}