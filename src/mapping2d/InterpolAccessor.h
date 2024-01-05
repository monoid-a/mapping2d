#ifndef MAPPING2D_MAPPING2D_INTERPOLACCESSOR_H_
#define MAPPING2D_MAPPING2D_INTERPOLACCESSOR_H_

template<typename Impl>
class InterpolAccessor
{
public:
	static UblDblMatrix calcMatrix(const Impl* impl)
	{
		return impl->calcMatrix();
	}

	static std::vector<double> getWeights(const Impl* impl, double x, double y)
	{
		return impl->getWeights(x, y);
	}

	static std::vector<double> getSampleValues(const Impl* impl, double x, double y)
	{
		return impl->getSampleValues(x, y);
	}

	static double correctZ(const Impl* impl, double z)
	{
		return impl->correctZ(z);
	}
};

#endif // MAPPING2D_MAPPING2D_INTERPOLACCESSOR_H_