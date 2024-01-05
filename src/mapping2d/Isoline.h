#ifndef MAPPING2D_MAPPING2D_ISOLINE_H_
#define MAPPING2D_MAPPING2D_ISOLINE_H_

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

	std::vector<Point>& getPoints();

private:
	std::vector<Point> mPoints;
};

#endif // MAPPING2D_MAPPING2D_ISOLINE_H_