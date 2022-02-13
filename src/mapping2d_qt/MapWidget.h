#pragma once

#include <QWidget>
#include "../mapping2d/RegularMesh2d.h"
#include "../mapping2d/MethodSettings.h"

class RegularMesh2d;
struct  PointsData;
class Surface;

class MapWidget : public QWidget
{
	Q_OBJECT

public:
	explicit MapWidget(QWidget* parent);
	~MapWidget();

	void calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny);
	void setDrawGrid(bool b);

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	void _drawPoints(QPainter& painter);
	void _drawGrid(QPainter& painter);
	void _drawSurface(QPainter& painter);
	void _drawScale(QPainter& painter);
	void _drawScene();

	void paintEvent(QPaintEvent* ev);
	void wheelEvent(QWheelEvent* ev);

	double transform_x(double x);
	double transform_y(double y);

	double inv_transform_x(double x);
	double inv_transform_y(double y);

private:
	double m_scale;
	double m_deltaScale;
	std::pair<double, double> m_translate;
	std::pair<double, double> m_center;
	RegularMesh2d m_mesh;
	PointsData* m_points;
	std::unique_ptr<Surface> m_surface;
	double m_xMax;
	double m_xMin;
	double m_yMax;
	double m_yMin;
	double m_zMin;
	double m_zMax;
	bool m_drawGrid;
	std::pair<int, int> m_prevPos;
};