#pragma once

class InterpolAccessor
{
public:
	template<typename Impl>
	static UblDblMatrix calcMatrix(Impl* impl)
	{
		return impl->_calcMatrix();
	}

	template<typename Impl>
	static std::vector<double> getWeights(Impl* impl, double x, double y)
	{
		return impl->_getWeights(x, y);
	}

	template<typename Impl>
	static std::vector<double> getVals(Impl* impl)
	{
		return impl->_getVals();
	}

	template<typename Impl>
	static double correctZ(Impl* impl, double z)
	{
		return impl->_correctZ(z);
	}
};
