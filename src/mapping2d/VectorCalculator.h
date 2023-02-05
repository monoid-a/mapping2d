#pragma once

#include "Structs.h"

class MAPPING2D VectorCalculator
{
public:
	static UblDblVec calculate(double x, double y, const PointsData& data, size_t n, const two_points_func& gamma, const modif_vector& vect_func, double init_val = 0.0);
};