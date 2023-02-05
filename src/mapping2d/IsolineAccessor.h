#pragma once

#include "Isoline.h"

class Surface;

class IsolineAccessor
{
public:
	template<typename Impl>
	static std::vector<Isoline> getIsolines(Impl* impl, double isovalue)
	{
		return impl->getIsolines(isovalue);
	}
};