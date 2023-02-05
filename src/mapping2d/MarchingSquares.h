#pragma once

#include "Isoliner.h"

class Surface;

class MAPPING2D MarchingSquares : public Isoliner<MarchingSquares>
{
	friend class IsolineAccessor;

public:
	explicit MarchingSquares(Surface* surface);
	~MarchingSquares();

protected:
	std::vector<Isoline> getIsolines(double isovalue);
};