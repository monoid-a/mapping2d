#ifndef MAPPING2D_MAPPING2D_ISOVALUES_H_
#define MAPPING2D_MAPPING2D_ISOVALUES_H_


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

#endif // MAPPING2D_MAPPING2D_ISOVALUES_H_