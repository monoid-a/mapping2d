#pragma once

#include "Structs.h"
#include "InterpolAccessor.h"
#include "MatrixCalculator.h"

template<typename Impl>
class Variogramer
{
public:
	explicit Variogramer(const two_points_func& cov) : mGamma(cov)
	{
		initMatrix();
	}

	Variogramer() = delete;

	void initMatrix()
	{
		Impl* impl = static_cast<Impl*>(this);
		UblDblMatrix A = InterpolAccessor::calcMatrix(impl);
		mInvA.resize(A.size1(), A.size1(), 0.0);
		MatrixCalculator::invertMatrix(A, mInvA);
	}

protected:
	two_points_func mGamma;
	UblDblMatrix mInvA;
};