#include "pch.h"
#include "MapWidget.h"
#include "mapping2d/RegularMesh2d.h"
#include "mapping2d/Surface.h"
#include "mapping2d/Variograms.h"
#include "mapping2d/Mapping.h"

#include <chrono>

MapWidget::MapWidget(QWidget* parent) : QWidget(parent)
{
	m_scale = 1.0;
	m_deltaScale = 1.0;
	m_points = nullptr;
	m_surface = nullptr;
	m_center = { 0.0, 0.0 };
	m_xMax = 0.0;
	m_xMin = 0.0;
	m_yMax = 0.0;
	m_yMin = 0.0;
	m_drawGrid = false;
	m_prevPos = std::make_pair(-1, -1);
}

MapWidget::~MapWidget()
{
}

void MapWidget::calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny)
{
	if (ps->x.empty() || ps->y.empty() || ps->z.empty())
		return;

#ifdef _DEBUG
	auto start = std::chrono::high_resolution_clock::now();
#endif

	m_points = ps;

	m_mesh = RegularMesh2d::calcRegularMesh2d(*ps, nx, ny);

	m_xMax = m_mesh.getXMax();
	m_yMax = m_mesh.getYMax();

	double xMin = m_mesh.getXMin();
	double yMin = m_mesh.getYMin();

	int w = width();
	int h = height();

	int screen_min = std::minmax(w, h).first;

	m_scale = std::minmax(m_xMax - xMin, m_yMax - yMin).first;

	m_scale = 0.75 * screen_min / m_scale;

	m_deltaScale = 0.1 * m_scale;

	m_translate = { 0.5 * width() , 0.5 * height() };

	m_center = { xMin + 0.5 * (m_xMax - xMin) , yMin + 0.5 * (m_yMax - yMin) };

	m_surface = Mapping::calculateSurface(m_points, settings, nx, ny);

#ifdef _DEBUG
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

//	QMessageBox msgBox;
//	QString val(std::to_string(duration.count()).c_str());
//	msgBox.setText(val + " milliseconds.");
//	msgBox.exec();
#endif

	m_zMin = m_surface->getZMin();
	m_zMax = m_surface->getZMax();

	m_prevPos = std::make_pair(-1, -1);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_mesh.getNx() == 0 || m_mesh.getNy() == 0)
		return;

	if (event->button() == Qt::LeftButton)
	{
		QPoint point = event->pos();
		double x = inv_transform_x(point.x());
		double y = inv_transform_y(point.y());
		Node node = m_mesh.getIJ(x, y);

		double z = m_surface->getZ(node.i, node.j);
		QString val(std::to_string(z).c_str());
		val = "Surface value: " + val;
		val = "Point: " + QString(std::to_string(x).c_str()) + ", " + QString(std::to_string(y).c_str()) + 
			"\n" + val;
		val = "Node: " + QString(std::to_string(node.i).c_str()) + ", " + QString(std::to_string(node.j).c_str()) +
			"\n" + val;

		QToolTip::showText(this->mapToGlobal(point), val, this);
	}

	if (event->button() == Qt::RightButton)
		m_prevPos = std::make_pair(-1, -1);
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::RightButton)
	{
		QPoint point = event->pos();
		if (m_prevPos.first == -1 && m_prevPos.second == -1)
		{
			m_prevPos.first = point.x();
			m_prevPos.second = point.y();
			return;
		}

		double x = inv_transform_x(point.x());
		double y = inv_transform_y(point.y());

		double prev_x = inv_transform_x(m_prevPos.first);
		double prev_y = inv_transform_y(m_prevPos.second);

		double dx = prev_x - x;
		double dy = prev_y - y;

		m_center.first += dx;
		m_center.second += dy;

		m_prevPos.first = point.x();
		m_prevPos.second = point.y();

		update();
	}
}

void MapWidget::setDrawGrid(bool b)
{
	m_drawGrid = b;
	update();
}

double MapWidget::inv_transform_x(double x)
{
	return m_center.first + (x - m_translate.first) / m_scale;
}

double MapWidget::inv_transform_y(double y)
{
	return m_center.second - (y - m_translate.second) / m_scale;
}

double MapWidget::transform_x(double x)
{
	return (x - m_center.first) * m_scale + m_translate.first;
}

double MapWidget::transform_y(double y)
{
	return (-y + m_center.second) * m_scale + m_translate.second;
}

void MapWidget::_drawGrid(QPainter& painter)
{
	if (!m_drawGrid)
		return;

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(Qt::black);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter.setPen(pen);

	size_t nx = m_mesh.getNx();
	size_t ny = m_mesh.getNy();

	for (size_t i = 0; i < nx; i++)
	{
		Point p0 = m_mesh.getXY(i, 0);
		Point p1 = m_mesh.getXY(i, ny - 1);

		int x0 = transform_x(p0.x);
		int y0 = transform_y(p0.y);

		int x1 = transform_x(p1.x);
		int y1 = transform_y(p1.y);

		painter.drawLine(x0, y0, x1, y1);
	}
	
	for (size_t j = 0; j < ny; j++)
	{
		Point p0 = m_mesh.getXY(0, j);
		Point p1 = m_mesh.getXY(nx - 1, j);

		int x0 = transform_x(p0.x);
		int y0 = transform_y(p0.y);

		int x1 = transform_x(p1.x);
		int y1 = transform_y(p1.y);

		painter.drawLine(x0, y0, x1, y1);
	}
}

void MapWidget::_drawSurface(QPainter& painter)
{
	if (!m_surface)
		return;

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(Qt::black);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter.setPen(pen);

	size_t nx = m_mesh.getNx();
	size_t ny = m_mesh.getNy();

	Qt::BrushStyle brushStyle = Qt::SolidPattern;

	double dx = m_mesh.getDx();
	double dy = m_mesh.getDy();

	for (size_t i = 0; i < nx; i++)
	{
		for (size_t j = 0; j < ny; j++)
		{
			Point p0 = m_mesh.getXY(i, j);
			Point p1;
			p1.x = p0.x + dx;
			p1.y = p0.y + dy;

			double x0 = transform_x(p0.x);
			double y0 = transform_y(p0.y);

			double x1 = transform_x(p1.x);
			double y1 = transform_y(p1.y);

			double dx = x1 - x0;
			double dy = y1 - y0;

			QRectF rect(x0 - 0.5 * (qreal)dx, y0 - 0.5 * (qreal)dy, (qreal)dx, (qreal)dy);

			double z = m_surface->getZ(i, j);
			double hue = (240.0 / 360.0) * (z - m_zMin) / (m_zMax - m_zMin);
			double s = 1.0;
			QColor colour = QColor::fromHsvF(qreal(hue), qreal(s), qreal(1.0));

			QBrush brush(colour, brushStyle);
			painter.fillRect(rect, brush);
		}
	}
}

void MapWidget::_drawPoints(QPainter& painter)
{
	if (!m_points)
		return;

	Qt::BrushStyle style = Qt::SolidPattern;
	QBrush brush(Qt::black, style);
	painter.setBrush(brush);

	size_t _size = m_points->x.size();

	for (size_t i = 0; i < _size; ++i)
	{
		double x = m_points->x[i];
		double y = m_points->y[i];

		double pX = transform_x(x);
		double pY = transform_y(y);

		painter.setPen(Qt::NoPen);
		painter.drawEllipse(pX, pY, 8, 8);

		QString name(std::to_string(i).c_str());
		painter.setFont(QFont("Arial", 7));
		QPen pen;
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(4);
		pen.setBrush(Qt::black);
		painter.setPen(pen);
		painter.drawText(QPoint(pX + 10, pY - 1), name);

		QString val(std::to_string(m_points->z[i]).c_str());
		painter.drawText(QPoint(pX + 10, pY + 15), val);
	}
}

void MapWidget::_drawScale(QPainter& painter)
{

}

void MapWidget::_drawScene()
{
	QPainter painter(this);

	int w = width();
	int h = height();

	painter.setWindow(0, 0, w, h);

	Qt::BrushStyle brushStyle = Qt::SolidPattern;

	QBrush backgroundBrush(Qt::white, brushStyle);

	painter.setBrush(backgroundBrush);
	QRect rect(0, 0, w, h);
	painter.fillRect(rect, backgroundBrush);

	_drawSurface(painter);
	_drawPoints(painter);
	_drawGrid(painter);
	_drawScale(painter);
}

void MapWidget::paintEvent(QPaintEvent* ev)
{
	_drawScene();
	__super::paintEvent(ev);
}

void MapWidget::wheelEvent(QWheelEvent* ev)
{
	QPoint numDegrees = ev->angleDelta();

	double y = numDegrees.y();
	y /= abs(y);
	y *= m_deltaScale;
	if (m_scale - y <= 0)
		return __super::wheelEvent(ev);

	m_scale -= y;

	ev->accept();

	update();

	__super::wheelEvent(ev);
}