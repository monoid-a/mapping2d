#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D SimpleKriging : public LinearEstimator<SimpleKriging>, public Variogramer<SimpleKriging>
{
	friend class InterpolAccessor;

public:
	SimpleKriging(const PointsData& data, const two_points_func& cov, double mean);
	SimpleKriging(PointsData&& data, const two_points_func& cov, double mean);
	~SimpleKriging();

protected:
	// interpol accessor
	std::vector<double> _getWeights(double x, double y);
	std::vector<double> _getVals();
	UblDblMatrix _calcMatrix();
	double _correctZ(double z);

	UblDblVec _calcVec(double x, double y, const PointsData& data, const two_points_func& gamma);
	static UblDblVec _calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b);

private:
	double m_mean;
};
