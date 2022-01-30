#include "pch.h"
#include "UniversalKriging.h"
#include "Structs.h"
#include "MatrixCalculator.h"
#include "VectorCalculator.h"

UniversalKriging::UniversalKriging(const PointsData& data, const two_points_func& cov) : LinearEstimator(data), Variogramer(cov)
{
}

UniversalKriging::UniversalKriging(PointsData&& data, const two_points_func& cov) : LinearEstimator(std::move(data)), Variogramer(cov)
{
}

UniversalKriging::~UniversalKriging()
{
}

UblDblVec UniversalKriging::_calcVec(double x, double y, const PointsData& data, const two_points_func& gamma)
{
	return VectorCalculator::calcVector(x, y, data, data.x.size() + 3, gamma,
		[x, y](UblDblVec& vec)
		{
			vec[vec.size() - 3] = 1.0;
			vec[vec.size() - 2] = x;
			vec[vec.size() - 1] = y;
		});
}

UblDblVec UniversalKriging::_calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b)
{
	auto w = prod(A_inv, b);
	return w;
}

std::vector<double> UniversalKriging::_getWeights(double x, double y)
{
	UblDblVec b = _calcVec(x, y, mPointsData, mGamma);
	UblDblVec w = _calcWeights(mInvA, b);
	return { w.begin() , w.end() };
}

std::vector<double> UniversalKriging::_getVals()
{
	std::vector<double> res;
	for (auto z : mPointsData.z)
		res.push_back(z);
	return res;
}

UblDblMatrix UniversalKriging::_calcMatrix()
{
	const auto& xs = mPointsData.x;
	const auto& ys = mPointsData.y;
	const size_t nx = xs.size() + 3;
	const size_t ny = ys.size() + 3;
	const size_t size = xs.size();

	UblDblMatrix matr = MatrixCalculator::calcMatrix(xs, ys, nx, ny, mGamma, [&xs, &ys](UblDblMatrix& matr)
		{
			for (size_t k = 0, p_cnt = xs.size(); k < p_cnt; ++k)
			{
				matr(k, xs.size()) = 1;
				matr(xs.size(), k) = 1;

				matr(k, xs.size() + 1) = xs[k];
				matr(xs.size() + 1, k) = xs[k];

				matr(k, ys.size() + 2) = ys[k];
				matr(ys.size() + 2, k) = ys[k];
			}
		});

	return matr;
}

double UniversalKriging::_correctZ(double z)
{
	return z;
}