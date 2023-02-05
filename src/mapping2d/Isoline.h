#pragma once

#include "Structs.h"

class MAPPING2D Isoline
{
public:
	Isoline();
	~Isoline();

	void add(const Segment& seg);
	void push_front(Point point);
	void push_back(Point point);
	Point get(size_t index) const;
	size_t getSize() const;
	void reverse();

private:
	std::deque<Point> mPoints;
};