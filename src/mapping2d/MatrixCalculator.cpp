#include "pch.h"
#include "MatrixCalculator.h"

UblDblMatrix MatrixCalculator::calcMatrix(const std::vector<double>& xs, const std::vector<double>& ys, size_t nx, size_t ny, const two_points_func& gamma, const matrix_modif& matrix_func, double init_val)
{
	UblDblMatrix matr(nx, ny, init_val);

	for (size_t i = 0, p_cnt = xs.size(); i < p_cnt; ++i)
	{
		double xi = xs[i];
		double yi = ys[i];
		for (size_t j = 0; j <= i; ++j)
		{
			double xj = xs[j];
			double yj = ys[j];
			matr(i, j) = gamma(Point{ xi, yi }, Point{ xj, yj });
			matr(j, i) = matr(i, j);
		}
	}

	if (matrix_func)
		matrix_func(matr);

	return matr;
}

bool MatrixCalculator::invertMatrix(const UblDblMatrix& input, UblDblMatrix& inverse)
{
	try
	{
		using pmatrix = ubl::permutation_matrix<UblDblMatrix::size_type, std::vector<UblDblMatrix::size_type>>;

		UblDblMatrix A(input);

		pmatrix pm(A.size1());

		size_t res = lu_factorize(A, pm);
		if (res != 0)
			return false;

		inverse.assign(ubl::identity_matrix<double>(A.size1()));

		lu_substitute(A, pm, inverse);
	}
	catch (...)
	{
		return false;
	}

	return true;
}