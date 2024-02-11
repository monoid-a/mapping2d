#ifndef MAPPING2D_MAPPING2D_QT_MAPWIDGET_H_
#define MAPPING2D_MAPPING2D_QT_MAPWIDGET_H_


#include <QWidget>

#include "../mapping2d/RegularMesh2d.h"
#include "../mapping2d/MethodSettings.h"
#include "../mapping2d/Isoline.h"
#include "../mapping2d/Surface.h"

class RegularMesh2d;
struct PointsData;
class Surface;
class Scale;

class MapWidget : public QWidget
{
	using super = QWidget;
	friend class Scale;

	Q_OBJECT

public:
	explicit MapWidget(QWidget* parent);
	~MapWidget();

	void calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny);
	void calculateSurface(PointsData* ps, MethodSettings settings, const RegularMesh2d& mesh);
	void calculateSurface(PointsData* ps, MethodSettings settings);
	void setDrawPoints(bool b);
	void setDrawGrid(bool b);
	void setDiscreteFill(bool b);
	void setContinuousFill(bool b);
	void saveSurface();
	void loadSurface();
	void calculateAndUpdateIsolines(double minz, double maxz, int levelCount);

signals:
	void onSurfCalculated(std::pair<double, double> minmax);
	void onSurfLoaded(std::pair<double, double> minmax);

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	void drawPoints(QPainter& painter);
	void drawGrid(QPainter& painter);
	void drawSurface(QPainter& painter);
	void drawScale(QPainter& painter);
	void drawIsolines(QPainter& painter);
	void drawIsobands(QPainter& painter);
	void drawScene();

	void paintEvent(QPaintEvent* ev);
	void wheelEvent(QWheelEvent* ev);

	double transform_x(double x);
	double transform_y(double y);

	double inv_transform_x(double x);
	double inv_transform_y(double y);

	Point transform_xy(double x, double y);
	Point inv_transform_xy(double x, double y);

	void initView();

	void calculateIsolines(double minz, double maxz, int levelCount);

private:
	double mScaleVal;
	double mDeltaScale;
	std::pair<double, double> mTranslate;
	std::pair<double, double> mCenter;
	RegularMesh2d mMesh;
	PointsData* mPoints;
	std::unique_ptr<Surface> mSurface;
	std::vector<std::pair<double, Isoline>> mIsolines;
	double mXMax;
	double mXMin;
	double mYMax;
	double mYMin;
	double mZMin;
	double mZMax;
	bool mDrawPoints;
	bool mDrawGrid;
	bool mDiscreteFill;
	bool mContinuousFill;
	std::pair<int, int> mPrevPos;
	std::unique_ptr<Scale> mScale;
};

#endif // MAPPING2D_MAPPING2D_QT_MAPWIDGET_H_