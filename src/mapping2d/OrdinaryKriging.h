#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D OrdinaryKriging : public LinearEstimator<OrdinaryKriging>, public Variogramer<OrdinaryKriging>
{
	friend class InterpolAccessor;

public:
	OrdinaryKriging(const PointsData& data, const two_points_func& cov);
	OrdinaryKriging(PointsData&& data, const two_points_func& cov);
	~OrdinaryKriging();

protected:
	// interpol accessor
	UblDblMatrix _calcMatrix();
	std::vector<double> _getWeights(double x, double y);
	std::vector<double> _getVals();
	double _correctZ(double z);

	UblDblVec _calcVec(double x, double y, const PointsData& data, const two_points_func& gamma);
	static UblDblVec _calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b);
};