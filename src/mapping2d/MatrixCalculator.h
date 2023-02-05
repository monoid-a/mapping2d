#pragma once

#include "Structs.h"

class MAPPING2D MatrixCalculator
{
public:
	static UblDblMatrix calcMatrix(const std::vector<double>& xs, const std::vector<double>& ys, size_t nx, size_t ny, const two_points_func& gamma, const modif_matrix& matrix_func, double init_val = 0.0);
	static bool invertMatrix(const UblDblMatrix& input, UblDblMatrix& inverse);
};