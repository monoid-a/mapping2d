#include "stdafx.h"

#include <iostream>
#include <vector>
#include <cassert>

#include "Structs.h"

#include "OrdinaryKriging.h"
#include "SimpleKriging.h"
#include "Variograms.h"

int main()
{
	PointsData points;
	points.x = { 1.0 , 1.5 , 2.3, 0.5 };
	points.y = { 0.0 , 3.5 , 4.3, 5.5 };
	points.z = { 20.0 , 11.5 , 12.3, 9.5 };

	SphericalVar sph_var;

	OrdinaryKriging ordKriging(points, sph_var);
	SimpleKriging simpleKriging(points, sph_var, 11.0);

	double okz = ordKriging.getZ(2.1, 1.2);
	double skz = simpleKriging.getZ(2.1, 1.2);

	return 0;
}