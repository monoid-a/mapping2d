#ifndef MAPPING2D_MAPPING2D_VECTORCALCULATOR_H_
#define MAPPING2D_MAPPING2D_VECTORCALCULATOR_H_

#include "Structs.h"

class MAPPING2D VectorCalculator
{
public:
	static UblDblVec calculate(double x, double y, const PointsData& data, size_t n, const TwoPointsFunc& gamma, const VectorModifFunc& vect_func, double init_val = 0.0);
};

#endif // MAPPING2D_MAPPING2D_VECTORCALCULATOR_H_