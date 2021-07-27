#pragma once

#include "Structs.h"
#include "MethodSettings.h"
#include "Surface.h"

class MAPPING2D Mapping
{
public:
	Mapping() = default;
	~Mapping() = default;

	static std::unique_ptr<Surface> calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny);
	static two_points_func getFunc(MethodSettings settings);
};