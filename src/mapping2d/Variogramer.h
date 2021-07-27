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
		mA = InterpolAccessor::calcMatrix(impl);
		mInvA.resize(mA.size1(), mA.size1(), 0.0);
		MatrixCalculator::invertMatrix(mA, mInvA);
	}

protected:
	two_points_func mGamma;
	UblDblMatrix mA;
	UblDblMatrix mInvA;
};