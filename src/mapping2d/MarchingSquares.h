#ifndef MAPPING2D_MAPPING2D_MARCHINGSQUARES_H_
#define MAPPING2D_MAPPING2D_MARCHINGSQUARES_H_

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

#endif // MAPPING2D_MAPPING2D_MARCHINGSQUARES_H_