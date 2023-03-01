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
	UblDblMatrix calcMatrix();
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getVals() const;
	double correctZ(double z) const;

	UblDblVec calcVec(double x, double y, const PointsData& data, const two_points_func& gamma) const;
	static UblDblVec calcWeights(const UblDblMatrix& A_inv, const UblDblVec& b);
};