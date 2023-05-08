#pragma once

class InterpolAccessor
{
public:
	template<typename Impl>
	static UblDblMatrix calcMatrix(Impl* impl)
	{
		return impl->calcMatrix();
	}

	template<typename Impl>
	static std::vector<double> getWeights(Impl* impl, double x, double y)
	{
		return impl->getWeights(x, y);
	}

	template<typename Impl>
	static std::vector<double> getVals(Impl* impl, double x, double y)
	{
		return impl->getVals(x, y);
	}

	template<typename Impl>
	static double correctZ(Impl* impl, double z)
	{
		return impl->correctZ(z);
	}
};
