#ifndef MAPPING2D_MAPPING2D_MAPPER_H_
#define MAPPING2D_MAPPING2D_MAPPER_H_

#include "Structs.h"
#include "MethodSettings.h"
#include "Surface.h"

class MAPPING2D Mapper
{
public:
	Mapper() = default;
	~Mapper() = default;

	static std::unique_ptr<Surface> calculateSurface(PointsData* ps, MethodSettings settings, const RegularMesh2d& mesh);
	static TwoPointsFunc getFunc(MethodSettings settings);
};

#endif // MAPPING2D_MAPPING2D_MAPPER_H_