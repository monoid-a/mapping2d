#include "pch.h"
#include "Isoline.h"

Isoline::Isoline()
{
}

Isoline::~Isoline()
{
}

void Isoline::add(const Segment& seg)
{
	if (mPoints.empty())
	{
		push_back(seg.bgn);
		push_back(seg.end);
	}
	else
	{
		if (mPoints[mPoints.size() - 1] == seg.bgn || mPoints[mPoints.size() - 1] == seg.end)
		{
			if (mPoints[mPoints.size() - 1] == seg.bgn)
				push_back(seg.end);
			else
				push_back(seg.bgn);
		}
		else if (mPoints[0] == seg.bgn || mPoints[0] == seg.end)
		{
			if (mPoints[0] == seg.bgn)
				push_front(seg.end);
			else
				push_front(seg.bgn);
		}
	}
}

void Isoline::push_front(Point point)
{
	mPoints.push_front(point);
}

void Isoline::push_back(Point point)
{
	mPoints.push_back(point);
}

Point Isoline::get(size_t index) const
{
	return mPoints[index];
}

size_t Isoline::getSize() const
{
	return mPoints.size();
}

void Isoline::reverse()
{
	std::reverse(mPoints.begin(), mPoints.end());
}