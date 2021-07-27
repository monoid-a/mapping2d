#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D UniversalKriging : public LinearEstimator<UniversalKriging>, public Variogramer<UniversalKriging>
{
	friend class InterpolAccessor;

public:
	UniversalKriging(const PointsData& data, const two_points_func& cov);
	UniversalKriging(PointsData&& data, const two_points_func& cov);
	~UniversalKriging();

protected:
	//interpol accessor
	std::vector<double> _getWeights(double x, double y);
	std::vector<double> _getVals();
	UblDblMatrix _calcMatrix();
	double _correctZ(double z);

	UblDblVec _calcVec(double x, double y, const PointsData& data, const two_points_func& gamma);
	static UblDblVec _calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b);
};
