#include "pch.h"
#include "Isoline.h"

Isoline::Isoline()
{
}

Isoline::Isoline(std::vector<Point>&& points) : mPoints(std::move(points))
{

}

Isoline::~Isoline()
{
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

std::vector<Point>& Isoline::getPoints()
{
	return mPoints;
}