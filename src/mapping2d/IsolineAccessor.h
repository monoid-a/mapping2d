#ifndef MAPPING2D_MAPPING2D_ISOLINEACCESSOR_H_
#define MAPPING2D_MAPPING2D_ISOLINEACCESSOR_H_

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

#endif // MAPPING2D_MAPPING2D_ISOLINEACCESSOR_H_