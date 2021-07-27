#pragma once

#include "Structs.h"

class VectorCalculator
{
public:
	static UblDblVec calcVector(double x, double y, const PointsData& data, size_t n, const two_points_func& gamma, const vector_modif& vect_func, double init_val = 0.0);
};