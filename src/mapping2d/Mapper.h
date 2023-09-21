#pragma once

#include "Structs.h"
#include "MethodSettings.h"
#include "Surface.h"

class MAPPING2D Mapper
{
public:
	Mapper() = default;
	~Mapper() = default;

	static std::unique_ptr<Surface> calculateSurface(PointsData* ps, MethodSettings settings, const RegularMesh2d& mesh);
	static two_points_func getFunc(MethodSettings settings);
};