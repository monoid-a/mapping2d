#pragma once

#include "Structs.h"

class MAPPING2D Isoline
{
public:
	Isoline();
	Isoline(std::vector<Point>&& points);
	~Isoline();

	void push_back(Point point);
	Point get(size_t index) const;
	size_t getSize() const;

private:
	std::vector<Point> mPoints;
};