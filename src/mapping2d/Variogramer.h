#ifndef MAPPING2D_MAPPING2D_VARIOGRAMER_H_
#define MAPPING2D_MAPPING2D_VARIOGRAMER_H_

#include "Structs.h"
#include "InterpolAccessor.h"
#include "MatrixCalculator.h"

template<typename Impl>
class Variogramer
{
public:
	explicit Variogramer(const TwoPointsFunc& cov) : mGamma(cov)
	{
		initMatrix();
	}

	Variogramer() = delete;

	void initMatrix()
	{
		const Impl* impl = static_cast<const Impl*>(this);
		UblDblMatrix A = InterpolAccessor<Impl>::calcMatrix(impl);
		mInvA.resize(A.size1(), A.size1(), 0.0);
		MatrixCalculator::invertMatrix(A, mInvA);
	}

protected:
	TwoPointsFunc mGamma; //!< RBF or Variogram
	UblDblMatrix mInvA; //!< inverse of the correlation/rbf matrix
};

#endif // MAPPING2D_MAPPING2D_VARIOGRAMER_H_