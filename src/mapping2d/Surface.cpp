#include "pch.h"
#include "Surface.h"
#include <numeric>

Surface::Surface(const RegularMesh2d& mesh) : mMesh(mesh)
{
	size_t nx = mMesh.getNx();
	mValues.resize(nx);
	for (size_t i = 0; i < nx; i++)
		mValues[i].resize(mMesh.getNy());
	mZMin = std::numeric_limits<double>::max();
	mZMax = std::numeric_limits<double>::lowest();
}

Surface::~Surface()
{

}

void Surface::setZ(size_t i, size_t j, double z)
{
	mValues[i][j] = z;
	mZMin = (std::min)(mZMin, z);
	mZMax = (std::max)(mZMax, z);
}

double Surface::getZ(size_t i, size_t j) const
{
	return mValues[i][j];
}

double Surface::getZMin() const
{
	return mZMin;
}

double Surface::getZMax() const
{
	return mZMax;
}

size_t Surface::getNx() const
{
	return mMesh.getNx();
}

size_t Surface::getNy() const
{
	return mMesh.getNy();
}

const RegularMesh2d& Surface::getMesh() const
{
	return mMesh;
}