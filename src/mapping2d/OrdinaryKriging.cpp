#include "pch.h"
#include "OrdinaryKriging.h"
#include "Structs.h"
#include "MatrixCalculator.h"
#include "VectorCalculator.h"

OrdinaryKriging::OrdinaryKriging(const PointsData& data, const two_points_func& cov) : LinearEstimator(data), Variogramer(cov)
{
}

OrdinaryKriging::OrdinaryKriging(PointsData&& data, const two_points_func& cov) : LinearEstimator(std::move(data)), Variogramer(cov)
{
}

OrdinaryKriging::~OrdinaryKriging()
{
}

UblDblMatrix OrdinaryKriging::_calcMatrix()
{
	const size_t nx = mPointsData.x.size() + 1;
	const size_t ny = mPointsData.y.size() + 1;

	const size_t size = mPointsData.x.size();

	UblDblMatrix matr = MatrixCalculator::calcMatrix(mPointsData.x, mPointsData.y, nx, ny, mGamma, [size](UblDblMatrix& matr)
		{
			for (size_t k = 0, p_cnt = size; k < p_cnt; ++k)
			{
				matr(k, size) = 1;
				matr(size, k) = matr(k, size);
			}
		});

	return matr;
}

UblDblVec OrdinaryKriging::_calcVec(double x, double y, const PointsData& data, const two_points_func& gamma)
{
	return VectorCalculator::calcVector(x, y, data, data.x.size() + 1, gamma,
		[](UblDblVec& vec)
		{
			vec[vec.size() - 1] = 1.0;
		});
}

UblDblVec OrdinaryKriging::_calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b)
{
	auto w = prod(A_inv, b);
	return w;
}

std::vector<double> OrdinaryKriging::_getWeights(double x, double y)
{
	UblDblVec b = _calcVec(x, y, mPointsData, mGamma);
	UblDblVec w = _calcWeights(mInvA, b);
	return { w.begin() , w.end() };
}

std::vector<double> OrdinaryKriging::_getVals()
{
	std::vector<double> res;
	for (auto v : mPointsData.z)
		res.push_back(v);
	return res;
}

double OrdinaryKriging::_correctZ(double z)
{
	return z;
}