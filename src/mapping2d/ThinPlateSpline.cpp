#include "pch.h"
#include "ThinPlateSpline.h"
#include "ThinPlateSplineRbf.h"
#include "Structs.h"
#include "MatrixCalculator.h"
#include "VectorCalculator.h"

ThinPlateSpline::ThinPlateSpline(const PointsData& data, double smoothParam) : LinearEstimator(data), Variogramer(TwoPointsFunc(ThinPlateSplineRbf{ smoothParam }))
{
	calcWeights();
}

ThinPlateSpline::ThinPlateSpline(PointsData&& data, double smoothParam) : LinearEstimator(std::move(data)), Variogramer(TwoPointsFunc(ThinPlateSplineRbf{ smoothParam }))
{
}

ThinPlateSpline::~ThinPlateSpline()
{

}

UblDblMatrix ThinPlateSpline::calcMatrix() const
{
	const auto& xs = mPointsData.x;
	const auto& ys = mPointsData.y;
	const size_t nx = xs.size() + 3;
	const size_t ny = ys.size() + 3;
	const size_t size = xs.size();

	UblDblMatrix matr = MatrixCalculator::calcMatrix(xs, ys, nx, ny, mGamma,
		[&xs, &ys](UblDblMatrix& matr)
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

void ThinPlateSpline::calcWeights()
{
	if (mWeights.empty())
	{
		UblDblVec f(mPointsData.z.size() + 3, 0.0);
		for (size_t i = 0, p_cnt = mPointsData.z.size(); i < p_cnt; ++i)
			f[i] = mPointsData.z[i];

		auto ws = prod(mInvA, f);

		mWeights.reserve(ws.size());
		for (auto w : ws)
			mWeights.push_back(w);
	}
}

std::vector<double> ThinPlateSpline::getWeights(double x, double y) const
{
	return mWeights;
}

std::vector<double> ThinPlateSpline::getSampleValues(double x, double y) const
{
	std::vector<double> vec(mPointsData.x.size() + 3, 0.0);

	for (size_t i = 0, p_cnt = mPointsData.x.size(); i < p_cnt; ++i)
		vec[i] = mGamma(Point{ x , y }, Point{ mPointsData.x[i] , mPointsData.y[i] });

	vec[vec.size() - 3] = 1.0;
	vec[vec.size() - 2] = x;
	vec[vec.size() - 1] = y;

	return vec;
}

double ThinPlateSpline::correctZ(double z) const
{
	return z;
}