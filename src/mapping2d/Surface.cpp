#include "stdafx.h"
#include "Surface.h"

Surface::Surface(size_t nx, size_t ny)
{
	m_values.resize(nx);
	for (size_t i = 0; i < nx; i++)
		m_values[i].resize(ny);
	m_zMin = FLT_MAX;
	m_zMax = -FLT_MAX;
}

Surface::~Surface()
{

}

void Surface::setZ(size_t i, size_t j, double z)
{
	m_values[i][j] = z;
	m_zMin = std::minmax(m_zMin, z).first;
	m_zMax = std::minmax(m_zMax, z).second;
}

double Surface::getZ(size_t i, size_t j)
{
	return m_values[i][j];
}

double Surface::getZMin()
{
	return m_zMin;
}

double Surface::getZMax()
{
	return m_zMax;
}