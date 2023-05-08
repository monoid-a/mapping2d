#pragma once

#include "LinearEstimator.h"
#include "Variogramer.h"

struct PointsData;

class MAPPING2D ThinPlateSpline : public LinearEstimator<ThinPlateSpline>, public Variogramer<ThinPlateSpline>
{
	friend class InterpolAccessor;

public:
	ThinPlateSpline(const PointsData& data);
	ThinPlateSpline(PointsData&& data);
	~ThinPlateSpline();

protected:
	//interpol accessor
	std::vector<double> getWeights(double x, double y) const;
	std::vector<double> getVals(double x, double y) const;
	
	UblDblMatrix calcMatrix();
	double correctZ(double z) const;
	void calcWeights();

protected:
	std::vector<double> mWeights;
};