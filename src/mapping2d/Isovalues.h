#pragma once

class Surface;

class MAPPING2D Isovalues
{
public:
	Isovalues(Surface* surf);
	~Isovalues();

	std::vector<double> getIsovalues(double step);
	std::vector<double> getIsovalues(int levelCount);
	std::vector<double> getIsovalues(double min, double max, int levelCount, double step);

private:
	Surface* mSurf;
};