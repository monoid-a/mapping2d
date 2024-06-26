#include "pch.h"
#include "VectorCalculator.h"

UblDblVec VectorCalculator::calculate(double x, double y, const PointsData& data, size_t n, const TwoPointsFunc& gamma, const VectorModifFunc& vect_func, double init_val)
{
	UblDblVec vec(n, 0.0);

	for (size_t i = 0, cnt = data.x.size(); i < cnt; ++i)
	{
		double xi = data.x[i];
		double yi = data.y[i];
		vec[i] = gamma(Point{ xi, yi }, Point{ x, y });
	}

	if (vect_func)
		vect_func(vec);

	return vec;
}