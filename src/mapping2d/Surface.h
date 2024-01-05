#ifndef MAPPING2D_MAPPING2D_SURFACE_H_
#define MAPPING2D_MAPPING2D_SURFACE_H_


#include "RegularMesh2d.h"

class MAPPING2D Surface
{
public:
	Surface(const RegularMesh2d& mesh);
	~Surface();

	void setZ(size_t i, size_t j, double z);
	double getZ(size_t i, size_t j) const;

	double getZMin() const;
	double getZMax() const;

	size_t getNx() const;
	size_t getNy() const;

	const RegularMesh2d& getMesh() const;

protected:
	RegularMesh2d mMesh;
	std::vector<std::vector<double>> mValues;
	double mZMin;
	double mZMax;
};

#endif // MAPPING2D_MAPPING2D_SURFACE_H_