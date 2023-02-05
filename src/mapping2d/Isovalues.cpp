#include "pch.h"
#include "Isovalues.h"
#include "Surface.h"

Isovalues::Isovalues(Surface* surf) : mSurf(surf)
{

}

Isovalues::~Isovalues()
{

}

std::vector<double> Isovalues::getIsovalues(double step)
{
	double min = mSurf->getZMin();
	min = std::round(min);

	double max = mSurf->getZMax();
	max = std::round(max);

	int levelCount = (int) (max - min) / step;

	return getIsovalues(min, max, levelCount, step);
}

std::vector<double> Isovalues::getIsovalues(int levelCount)
{
	double min = mSurf->getZMin();
	min = std::round(min);

	double max = mSurf->getZMax();
	max = std::round(max);

	double step = (max - min) / levelCount;

	return getIsovalues(min, max, levelCount, step);
}

std::vector<double> Isovalues::getIsovalues(double min, double max, int levelCount, double step)
{
	std::vector<double> res;
	for (int i = 0; i <= levelCount; ++i)
	{
		double p = min + i * step;
		res.push_back(p);
	}
	return res;
}