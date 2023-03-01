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
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getVals() const;
	UblDblMatrix calcMatrix();
	double correctZ(double z) const;

	UblDblVec calcVec(double x, double y, const PointsData& data, const two_points_func& gamma) const;
	static UblDblVec calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b);
};
