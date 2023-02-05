#pragma once

#include "Isoline.h"
#include "Isovalues.h"
#include "IsolineAccessor.h"

class Surface;

template<typename Impl>
class Isoliner
{
public:
	explicit Isoliner(Surface* surface) : mSurface(surface)
	{
	}

	~Isoliner()
	{
	}

	std::vector<std::pair<double, Isoline>> calculate(double min, double max, int levelCount, double step)
	{
		Impl* impl = static_cast<Impl*>(this);

		Isovalues isovalues(mSurface);

		std::vector<double> vals = isovalues.getIsovalues(min, max, levelCount, step);

		std::vector<std::pair<double, Isoline>> res;

		for (size_t i = 0; i < vals.size(); ++i)
		{
			double isovalue = vals[i];
			std::vector<Isoline> isolines = IsolineAccessor::getIsolines(impl, isovalue);
			for (auto& iso : isolines)
				res.push_back({ isovalue, std::move(iso) });
		}

		return res;
	}

protected:
	Surface* mSurface;
};