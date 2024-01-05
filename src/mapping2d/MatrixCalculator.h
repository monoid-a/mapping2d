#ifndef MAPPING2D_MAPPING2D_MATRIXCALCULATOR_H_
#define MAPPING2D_MAPPING2D_MATRIXCALCULATOR_H_

#include "Structs.h"

class MAPPING2D MatrixCalculator
{
public:
	static UblDblMatrix calcMatrix(const std::vector<double>& xs, const std::vector<double>& ys, size_t nx, size_t ny, const TwoPointsFunc& gamma, const MatrixModifFunc& matrix_func, double init_val = 0.0);
	static bool invertMatrix(const UblDblMatrix& input, UblDblMatrix& inverse);
};

#endif // MAPPING2D_MAPPING2D_MATRIXCALCULATOR_H_