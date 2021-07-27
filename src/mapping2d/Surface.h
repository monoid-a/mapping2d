#pragma once

#include "RegularMesh2d.h"

class MAPPING2D Surface
{
public:
	Surface(size_t ni, size_t nj);
	~Surface();

	void setZ(size_t i, size_t j, double z);
	double getZ(size_t i, size_t j);

	double getZMin();
	double getZMax();

protected:
	std::vector<std::vector<double>> m_values;
	double m_zMin;
	double m_zMax;
};